#include <engine/context.hpp>
#include <game/cooker_pool.hpp>
#include <game/explode.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <glm/gtx/norm.hpp>
#include <util/random.hpp>
#include <util/util.hpp>
#include <algorithm>

namespace rr {
void CookerPool::spawn(Cooker const& cooker) {
	auto entry = Entry{};
	entry.trigger = {cooker.position, triggerDiameter};
	entry.cooker = cooker;

	entry.sprite = Sprite(game()->context, "cooker");
	entry.sprite.instance().transform.position = entry.trigger.centre;

	entry.text = vf::Text(game()->context.vfContext, "cooker");
	entry.text.setFont(&game()->resources().fonts.main).setHeight(textHeight).setString(util::formatElapsed(cooker.cook));
	entry.text.transform().position = entry.trigger.centre;
	entry.text.transform().position.y += 100.0f * basis().scale;

	m_entries.push_back(std::move(entry));
}

void CookerPool::tick(DeltaTime dt) {
	if (game()->state() != Game::State::ePlay) { return; }
	auto& player = game()->player();
	if (auto it = updateTriggersAndGetNearest(player.trigger, dt.scaled); it != m_entries.end()) {
		if (player.interact()) {
			player.collect(it->cooker.points);
			spawnCollect(it->trigger.centre);
			pop(*it);
		}
	}
	std::erase_if(m_entries, [this](Entry const& e) {
		if (e.cooker.ready <= 0s) {
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
		if (entry.cooker.cook <= 0s) { frame.draw(entry.text); }
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
		if (e->cooker.cook > 0s) { return; }
		auto const toT = glm::length2(e->sprite.instance().transform.position - player.centre);
		if (nearest.it == m_entries.end() || toT < nearest.sqrDist) { nearest = {e, toT}; }
	};

	player.interactable = false;
	for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
		it->trigger.interactable = false;
		if (player.intersecting(it->trigger)) { setNearest(it); }
		if (it->cooker.cook > 0s) {
			it->cooker.cook -= dt;
		} else {
			if (it->vibrateRemain <= 0s) {
				auto const dxy = glm::vec2(5.0f) * layout().basis.scale;
				it->sprite.instance().transform.position = it->cooker.position + util::randomRange(-dxy, dxy);
				it->vibrateRemain = vibrate;
			}
			it->vibrateRemain -= dt;
			it->cooker.ready -= dt;
			it->sprite.instance().tint = vf::Rgba::make(0xffffccff).linear();
			it->text.setString(util::formatElapsed(it->cooker.ready));
		}
	}

	if (nearest.it != m_entries.end()) { player.interactable = nearest.it->trigger.interactable = true; }
	return nearest.it;
}

void CookerPool::spawnCollect(glm::vec2) const {}

void CookerPool::pop(Entry& entry) {
	if (&entry != &m_entries.back()) { std::swap(entry, m_entries.back()); }
	m_entries.pop_back();
}
} // namespace rr
