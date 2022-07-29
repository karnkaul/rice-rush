#include <engine/audio.hpp>
#include <game/explode_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>

namespace rr {
void ExplodePool::spawn(glm::vec2 const position) {
	auto& entry = make_entry();
	entry.sprite.instance().transform.position = position;
	game()->audio().play(game()->resources.sfx.explode);
}

void ExplodePool::tick(DeltaTime dt) {
	auto& player = game()->player();
	for (auto& entry : m_active) {
		entry.sprite.tick(dt.scaled);
		if (entry.ttl.force > 0s) {
			auto const to_player = player.sprite.instance().transform.position - entry.sprite.instance().transform.position;
			if (auto const sqr_mag = glm::length2(to_player); sqr_mag > 0.0f) {
				auto const dir = to_player / std::sqrt(sqr_mag);
				player.translate(force.impulse * dt.scaled.count() * basis().scale * dir);
			}
			entry.ttl.force -= dt.scaled;
		}
		entry.ttl.anim -= dt.scaled;
	}
	std::erase_if(m_active, [&](Entry& e) {
		if (e.ttl.anim <= 0s) {
			m_inactive.push_back(std::move(e));
			return true;
		}
		return false;
	});
}

void ExplodePool::draw(vf::Frame const& frame) const {
	for (auto const& entry : m_active) { entry.sprite.draw(frame); }
}

auto ExplodePool::make_entry() -> Entry& {
	auto const& anim = game()->resources.animations.explode;
	if (m_inactive.empty()) {
		m_active.push_back({AnimatedSprite(game()->context, "explode")});
		m_active.back().sprite.set_size(basis().scale * size);
		logger::info("explode created");
	} else {
		auto entry = std::move(m_inactive.back());
		m_inactive.pop_back();
		m_active.push_back(std::move(entry));
	}
	auto& ret = m_active.back();
	ret.sprite.set_sheet(anim.sheet, anim.sequence);
	ret.ttl.force = force.duration;
	ret.ttl.anim = anim.sequence.duration;
	return ret;
}
} // namespace rr
