#include <engine/context.hpp>
#include <game/cooker_pool.hpp>
#include <game/explode.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <glm/gtx/norm.hpp>
#include <util/util.hpp>
#include <algorithm>

namespace rr {
void CookerPool::spawn(glm::vec2 const position, vf::Time const ttl, Points const points) {
	auto entry = Entry{};
	entry.trigger = {position, triggerDiameter};
	entry.ttl = ttl;
	entry.points = points;

	entry.sprite = Sprite(game()->context, "cooker");
	entry.sprite.instance().transform.position = position;

	entry.text = vf::Text(game()->context.vfContext, "cooker");
	entry.text.setFont(&game()->resources().fonts.main).setHeight(textHeight).setString(util::formatElapsed(ttl));
	entry.text.transform().position = entry.trigger.centre;
	entry.text.transform().position.y += 100.0f * basis().scale;

	m_entries.push_back(std::move(entry));
}

void CookerPool::tick(DeltaTime dt) {
	if (game()->state() != Game::State::ePlay) { return; }
	auto& player = game()->player();
	if (auto it = updateTriggersAndGetNearest(player.trigger, dt.scaled); it != m_entries.end()) {
		if (player.interact()) {
			player.collect(it->points);
			spawnCollect(it->trigger.centre);
			m_entries.erase(it);
		}
	}
	std::erase_if(m_entries, [this](Entry const& e) {
		if (e.ttl <= 0s) {
			game()->player().hit();
			game()->spawn<Explode>(e.trigger.centre);
			return true;
		}
		return false;
	});
}

void CookerPool::draw(vf::Frame const& frame) const {
	for (auto const& entry : m_entries) {
		entry.sprite.draw(frame);
		frame.draw(entry.text);
	}
}

void CookerPool::addTriggers(std::vector<Ptr<Trigger const>>& out) const {
	for (auto const& entry : m_entries) { out.push_back(&entry.trigger); }
}

auto CookerPool::updateTriggersAndGetNearest(Trigger& player, vf::Time dt) -> std::vector<Entry>::iterator {
	struct {
		std::vector<Entry>::iterator it{};
		float sqrDist{};
	} nearest{m_entries.end()};

	auto setNearest = [&](std::vector<Entry>::iterator const& e) {
		auto const toT = glm::length2(e->sprite.instance().transform.position - player.centre);
		if (nearest.it == m_entries.end() || toT < nearest.sqrDist) { nearest = {e, toT}; }
	};

	player.interactable = false;
	for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
		it->trigger.interactable = false;
		if (player.intersecting(it->trigger)) { setNearest(it); }
		it->text.setString(util::formatElapsed(it->ttl));
		it->ttl -= dt;
	}

	if (nearest.it != m_entries.end()) { player.interactable = nearest.it->trigger.interactable = true; }
	return nearest.it;
}

void CookerPool::spawnCollect(glm::vec2) const {}
} // namespace rr
