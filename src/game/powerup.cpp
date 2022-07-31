#include <engine/audio.hpp>
#include <game/cooker_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/powerup.hpp>
#include <game/resources.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>

namespace rr {
namespace {
struct Heal : Powerup::Modifier {
	Player& player;
	int hp;

	Heal(Player& player, int hp) : player(player), hp(hp) {}

	vf::Time enable() override {
		player.heal(hp);
		return {};
	}
};

struct Slomo : Powerup::Modifier {
	Game& game;
	float time_scale{0.5f};
	vf::Time effect{2s};

	Slomo(Game& game, float time_scale, vf::Time effect) : game(game), time_scale(time_scale), effect(effect) {}

	vf::Time enable() override {
		game.time_scale = time_scale;
		return effect;
	}

	void disable() override { game.time_scale = 1.0f; }
};

struct Sweep : Powerup::Modifier {
	CookerPool& cooker_pool;

	Sweep(CookerPool& cooker_pool) : cooker_pool(cooker_pool) {}

	vf::Time enable() override {
		cooker_pool.sweep_ready();
		return {};
	}
};
} // namespace

glm::vec2 Powerup::random_position(float const distance_factor, int const max_tries) const {
	auto ret = glm::vec2{};
	auto const zone = 0.4f * layout().play_area.extent;
	auto const player = game()->player().trigger.centre;
	auto const offset = layout().play_area.offset;
	auto const min_distance = distance_factor * layout().play_area.extent.x;
	auto const sqr_min_dist = min_distance * min_distance;
	for (int tries = 1; tries <= max_tries; ++tries) {
		ret = util::random_range(-zone, zone) + offset;
		if (auto const sqr_dist = glm::length2(ret - player); sqr_dist >= sqr_min_dist) {
			logger::debug("[Powerup] random_position(): success in [{}] tries", tries);
			return ret;
		}
	}
	logger::debug("[Powerup] random_position(): max tries exceeded: [{}]", max_tries);
	return ret;
}

bool Powerup::activate_sweep() {
	if (!can_activate()) { return false; }

	m_active.modifier = ktl::make_unique<Sweep>(*game()->cooker_pool());
	activate(&game()->resources.textures.powerups.sweep);
	logger::debug("[Powerup] Sweep activated; ttl: [{:.1f}s]", ttl.spawn.count());
	return true;
}

bool Powerup::activate_slowmo(float time_scale, vf::Time duration) {
	if (!can_activate()) { return false; }

	if (time_scale >= 1.0f) {
		logger::debug("[Powerup] slowmo time scale [{}] must be < 1, activation request denied", time_scale);
		return false;
	}
	if (duration <= 0s) {
		logger::debug("[Powerup] slowmo duration [{}s] must be > 0s, activation request denied", duration.count());
		return false;
	}

	m_active.modifier = ktl::make_unique<Slomo>(*game(), time_scale, duration);
	activate(&game()->resources.textures.powerups.slomo);
	logger::debug("[Powerup] SloMo activated; ttl: [{:.1f}s]", ttl.spawn.count());
	return true;
}

bool Powerup::activate_heal(int hp) {
	if (!can_activate()) { return false; }

	if (hp <= 0) {
		logger::debug("[Powerup] heal HP [{}] must be > 0, activation request denied", hp);
		return false;
	}

	m_active.modifier = ktl::make_unique<Heal>(game()->player(), hp);
	activate(&game()->resources.textures.powerups.heal);
	logger::debug("[Powerup] Heal activated; ttl: [{:.1f}s]", ttl.spawn.count());
	return true;
}

void Powerup::deactivate() {
	if (m_active.modifier) { m_active.modifier->disable(); }
	m_active = {};
	logger::debug("[Powerup] disabled");
}

void Powerup::setup() {
	m_sprite = AnimatedSprite(game()->context, "powerup");
	diameter *= basis().scale;
	layer = layers::powerup;
}

void Powerup::tick(DeltaTime dt) {
	if (!m_active.modifier) { return; }

	if (m_active.effect) {
		update_effect(dt.real);
	} else {
		update_active(dt.real);
	}
}

void Powerup::draw(vf::Frame const& frame) const {
	if (active() && !m_active.effect) { m_sprite.draw(frame); }
}

void Powerup::add_triggers(std::vector<Ptr<Trigger const>>& out) const {
	if (active() && !m_active.effect) { out.push_back(&m_trigger); }
}

bool Powerup::can_activate() const {
	if (active()) {
		logger::debug("[Powerup] Already active, activation request denied");
		return false;
	}
	return true;
}

void Powerup::activate(Ptr<vf::Texture const> texture) {
	m_sheet.set_texture(texture);
	m_sprite.set_size(glm::vec2{diameter}).set_sheet(&m_sheet);
	m_sprite.instance().transform.position = m_trigger.centre = random_position();
	m_trigger.diameter = diameter * (1.0f + squish_coeff);
	m_active.elapsed = {};
}

void Powerup::update_active(vf::Time dt) {
	auto const squish = squish_rate * m_active.elapsed.count();
	auto const delta = squish_coeff * glm::vec2{std::cos(squish), std::sin(squish)};
	m_sprite.instance().transform.scale = glm::vec2{1.0f} + delta;

	if (m_trigger.intersecting(game()->player().trigger)) {
		game()->audio().play(game()->resources.sfx.powerup);
		if (m_active.elapsed = m_active.modifier->enable(); m_active.elapsed <= 0s) {
			deactivate();
		} else {
			m_active.effect = true;
		}
		return;
	}

	m_active.elapsed += dt;
	if (m_active.elapsed >= ttl.spawn) { deactivate(); }
}

void Powerup::update_effect(vf::Time dt) {
	m_active.elapsed -= dt;
	if (m_active.elapsed <= 0s) {
		deactivate();
		return;
	}
}
} // namespace rr
