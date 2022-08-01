#include <engine/audio.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>

namespace rr {
namespace {
template <typename T>
Ptr<T> refresh(std::span<ktl::kunique_ptr<T> const> ts, Trigger& player) {
	struct {
		Ptr<T> t{};
		float sqrDist{};
	} nearest{};

	auto setNearest = [&player, &nearest](T& pawn) {
		auto const toT = glm::length2(pawn.sprite.instance().transform.position - player.centre);
		if (!nearest.t || toT < nearest.sqrDist) { nearest = {&pawn, toT}; }
	};

	player.interactable = false;
	for (auto& t : ts) {
		t->trigger.interactable = false;
		if (player.intersecting(t->trigger)) { setNearest(*t); }
	}

	if (nearest.t) { player.interactable = nearest.t->trigger.interactable = true; }
	return nearest.t;
}

constexpr glm::vec2 clamp(glm::vec2 in, vf::Rect const& playArea, glm::vec2 const pad) {
	auto const bl = playArea.bottomLeft() + pad;
	auto const tr = playArea.topRight() - pad;
	in.x = std::clamp(in.x, bl.x, tr.x);
	in.y = std::clamp(in.y, bl.y, tr.y);
	return in;
}
} // namespace

void Player::translate(glm::vec2 xy) {
	auto& pos = sprite.instance().transform.position;
	pos += xy;
	trigger.centre = pos = clamp(pos, game()->layout.play_area, sprite.quad().size() * 0.5f);
}

void Player::collect(std::uint32_t points, bool incrementMultiplier) {
	auto const dp = points * m_state.multiplier;
	m_state.score += dp;
	if (incrementMultiplier) { ++m_state.multiplier; }
	game()->audio().play(game()->resources.sfx.collect);
	logger::debug("[Player] collected {} points, X: {}", dp, m_state.multiplier);
}

void Player::hit() {
	if (m_state.health.hit()) {
		m_state.multiplier = std::max(m_state.multiplier / 2, 1U);
		logger::debug("[Player] hit; hp: {}, X: {}", m_state.health.hp, m_state.multiplier);
	}
}

void Player::heal(int hp) {
	m_state.health.heal(hp, max_hp);
	logger::debug("[Player] heal: {}; hp: {}", hp, m_state.health.hp);
}

void Player::reset(glm::vec2 const position) {
	logger::debug("[Player] reset");
	m_state = {};
	auto const& anim = game()->resources.animations.player;
	sprite.set_sheet(anim.sheet, anim.sequence).set_size(size);
	sprite.instance().transform.position = trigger.centre = position;
	sprite.instance().transform.scale = glm::vec2{1.0f};
}

void Player::setup() {
	sprite = AnimatedSprite(game()->context, "player");
	layer = layers::player;

	size *= basis().scale;
	trigger.diameter *= basis().scale;
}

void Player::tick(DeltaTime dt) {
	auto const cs = controller.update(game()->keyboard());
	m_state.flags = cs.flags;
	if (game()->state() != Game::State::ePlay) { return; }

	m_state.health.tick(dt.scaled);
	sprite.tick(dt.scaled);

	translate(cs.xy * speed * basis().scale * dt.scaled.count());
	if (std::abs(cs.xy.x) > 0.01f) {
		if (cs.xy.x < 0.0f) {
			sprite.instance().transform.scale.x = -1.0f;
		} else {
			sprite.instance().transform.scale.x = 1.0f;
		}
	}

	auto const alpha = static_cast<vf::Rgba::Channel>(m_state.health.is_immune() ? 0xbb : 0xff);
	sprite.instance().tint.channels[3] = alpha;
}
} // namespace rr
