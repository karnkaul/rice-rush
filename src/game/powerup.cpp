#include <game/game.hpp>
#include <game/player.hpp>
#include <game/powerup.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>

namespace rr {
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

bool Powerup::activate(Request request, glm::vec2 position) {
	if (m_request.modify) {
		logger::debug("[Powerup] Already active, activation request denied");
		return false;
	}
	if (!request.modify) {
		logger::debug("[Powerup] Modifier cannot be empty, activation request denied");
		return false;
	}

	m_request = request;
	if (m_request.sheet) {
		m_sprite.set_sheet(*m_request.sheet, m_request.sequence);
	} else {
		m_sprite.unset_sheet();
	}
	m_sprite.set_size(glm::vec2{m_request.diameter * 0.5f});

	m_sprite.instance().transform.position = m_trigger.centre = position;
	m_trigger.diameter = m_request.diameter;
	logger::debug("[Powerup] Modifier activated; ttl: [{:.1f}s]", m_request.ttl.count());
	return true;
}

void Powerup::deactivate() {
	logger::debug("[Powerup] Modifier deactivated");
	m_request.modify = {};
}

void Powerup::setup() {
	m_sprite = AnimatedSprite(game()->context, "powerup");
	layer = layers::powerup;
}

void Powerup::tick(DeltaTime dt) {
	if (!active()) { return; }

	m_sprite.tick(dt.scaled);
	if (m_trigger.intersecting(game()->player().trigger)) {
		m_request.modify(*game());
		deactivate();
		return;
	}

	m_request.ttl -= dt.real;
	if (m_request.ttl <= 0s) { deactivate(); }
}

void Powerup::draw(vf::Frame const& frame) const {
	if (active()) { m_sprite.draw(frame); }
}

void Powerup::add_triggers(std::vector<Ptr<Trigger const>>& out) const {
	if (active()) { out.push_back(&m_trigger); }
}
} // namespace rr
