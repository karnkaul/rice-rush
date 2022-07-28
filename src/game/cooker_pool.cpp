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
void CookerPool::set_prefab(Prefab prefab) {
	m_entries.mesh.gbo.write(vf::Geometry::makeQuad({prefab.size}));
	m_entries.mesh.texture = prefab.texture;
}

void CookerPool::spawn(Cooker const& cooker) {
	auto entry = Entry{};
	entry.trigger = {cooker.position, triggerDiameter};
	entry.cooker = cooker;

	entry.text = vf::Text(game()->context.vf_context, "cooker");
	entry.text.setFont(&game()->resources.fonts.main).setHeight(textHeight).setString(util::format_elapsed(cooker.cook));
	entry.text.transform().position = entry.trigger.centre;
	entry.text.transform().position.y += 100.0f * basis().scale;

	m_entries.add(cooker.position, std::move(entry));
}

void CookerPool::setup() {
	m_entries.mesh = {game()->context.vf_context, "cookers"};
	m_entries.mesh.gbo.write(vf::Geometry::makeQuad());
}

void CookerPool::tick(DeltaTime dt) {
	if (game()->state() != Game::State::ePlay) { return; }
	auto& player = game()->player();
	if (auto entry = update_and_get_nearest(player.trigger, dt.scaled)) {
		if (player.interact()) {
			player.collect(entry->t.cooker.points);
			spawn_collect(entry->t.trigger.centre);
			pop(*entry);
		}
	}
	std::erase_if(m_entries.entries, [this](Instanced<Entry>::Entry const& e) {
		if (e.t.cooker.ready <= 0s) {
			game()->player().hit();
			game()->spawn<Explode>(e.t.trigger.centre);
			return true;
		}
		return false;
	});
}

void CookerPool::draw(vf::Frame const& frame) const {
	m_entries.draw(frame);
	for (auto const& entry : m_entries.entries) {
		if (entry.t.cooker.cook <= 0s) { frame.draw(entry.t.text); }
	}
}

void CookerPool::add_triggers(std::vector<Ptr<Trigger const>>& out) const {
	for (auto const& entry : m_entries.entries) { out.push_back(&entry.t.trigger); }
}

auto CookerPool::update_and_get_nearest(Trigger& player, vf::Time dt) -> Ptr<Instanced<Entry>::Entry> {
	struct {
		Ptr<Instanced<Entry>::Entry> entry{};
		float sqrDist{};
	} nearest{};

	auto setNearest = [&](Instanced<Entry>::Entry& e) {
		if (e.t.cooker.cook > 0s) { return; }
		auto const toT = glm::length2(e.t.cooker.position - player.centre);
		if (!nearest.entry || toT < nearest.sqrDist) { nearest = {&e, toT}; }
	};

	player.interactable = false;
	for (auto& entry : m_entries.entries) {
		entry.t.trigger.interactable = false;
		if (player.intersecting(entry.t.trigger)) { setNearest(entry); }
		if (entry.t.cooker.cook > 0s) {
			entry.t.cooker.cook -= dt;
		} else {
			if (entry.t.vibrateRemain <= 0s) {
				auto const dxy = glm::vec2(5.0f) * layout().basis.scale;
				entry.instance.transform.position = entry.t.cooker.position + util::random_range(-dxy, dxy);
				entry.t.vibrateRemain = vibrate;
			}
			entry.t.vibrateRemain -= dt;
			entry.t.cooker.ready -= dt;
			entry.instance.tint = vf::Rgba::make(0xffffccff).linear();
			entry.t.text.setString(util::format_elapsed(entry.t.cooker.ready));
		}
	}

	if (nearest.entry) { player.interactable = nearest.entry->t.trigger.interactable = true; }
	return nearest.entry;
}

void CookerPool::spawn_collect(glm::vec2) const {}

void CookerPool::pop(Instanced<Entry>::Entry& entry) {
	if (&entry != &m_entries.entries.back()) { std::swap(entry, m_entries.entries.back()); }
	m_entries.entries.pop_back();
}
} // namespace rr
