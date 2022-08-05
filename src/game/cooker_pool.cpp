#include <engine/context.hpp>
#include <game/cooker_pool.hpp>
#include <game/explode_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <util/util.hpp>
#include <algorithm>

namespace rr {
glm::vec2 CookerPool::random_position(int max_tries) const {
	auto const offset = game()->layout.play_area.offset;
	auto const zone = 0.5f * game()->layout.play_area.extent - 2.0f * size;

	auto make_pos = [zone, offset] { return util::random_range(-zone, zone) + offset; };
	auto pos = make_pos();
	for (int loops{}; loops < max_tries && intersecting(rr::Trigger{pos, trigger_diameter}); ++loops) { pos = make_pos(); }
	return pos;
}

void CookerPool::spawn(Cooker const& cooker) {
	auto entry = Entry{};
	entry.trigger = {cooker.position, trigger_diameter};
	entry.cooker = cooker;

	entry.text = vf::Text(game()->context.vf_context);
	entry.text.set_font(&game()->resources.fonts.main).set_height(text_height).set_string(util::format_elapsed(cooker.cook));
	entry.text.transform().position = entry.trigger.centre;
	entry.text.transform().position.y += 0.75f * size.y;
	entry.text.tint() = text_tint;

	m_entries.add(cooker.position, std::move(entry));
}

void CookerPool::spawn() {
	auto const cook = util::random_range(ranges.cook.first, ranges.cook.second);
	auto const ready = util::random_range(ranges.ready.first, ranges.ready.second);
	spawn(Cooker{random_position(), cook, ready});
}

std::size_t CookerPool::sweep_ready() {
	auto const ret = std::erase_if(m_entries.entries, [](auto const& entry) { return entry.t.cooker.cook <= 0s; });
	logger::debug("[Cookers] swept [{}] ready cookers", ret);
	return ret;
}

bool CookerPool::intersecting(Trigger const& trigger) const {
	return std::any_of(m_entries.entries.begin(), m_entries.entries.end(), [&trigger](auto const& e) { return e.t.trigger.intersecting(trigger); });
}

void CookerPool::setup() {
	m_entries.mesh = Instanced<Entry>::Mesh{game()->context.vf_context};
	m_entries.mesh.buffer.write(vf::Geometry::make_quad({size * basis().scale}));
	m_entries.mesh.texture = game()->resources.textures.cooker.handle();

	m_sfx = game()->context.capo_instance->make_source();
	m_sfx.loop(true);
	m_sfx.bind(game()->resources.sfx.tick_tock);

	m_explode_pool = game()->spawn<ExplodePool>();

	size *= basis().scale;
	text_height = static_cast<vf::Text::Height>(static_cast<float>(text_height) * basis().scale);
	trigger_diameter *= basis().scale;
}

void CookerPool::tick(DeltaTime dt) {
	if (game()->state() != Game::State::ePlay) {
		if (m_sfx.state() == capo::State::ePlaying) { m_sfx.stop(); }
		return;
	}
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
			m_explode_pool->spawn(e.t.trigger.centre);
			return true;
		}
		return false;
	});
}

void CookerPool::draw(vf::Frame const& frame) const {
	m_entries.draw(frame);
	if (game()->state() == Game::State::ePlay) {
		for (auto const& entry : m_entries.entries) {
			if (entry.t.cooker.cook <= 0s) { frame.draw(entry.t.text); }
		}
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

	auto set_nearest = [&](Instanced<Entry>::Entry& e) {
		if (e.t.cooker.cook > 0s) { return; }
		auto const toT = glm::length2(e.t.cooker.position - player.centre);
		if (!nearest.entry || toT < nearest.sqrDist) { nearest = {&e, toT}; }
	};

	player.interactable = false;
	auto ticking{false};
	for (auto& entry : m_entries.entries) {
		entry.t.trigger.interactable = false;
		if (player.intersecting(entry.t.trigger)) { set_nearest(entry); }
		if (entry.t.cooker.cook > 0s) {
			entry.t.cooker.cook -= dt;
		} else {
			ticking = true;
			if (entry.t.vibrateRemain <= 0s) {
				auto const dxy = glm::vec2(5.0f) * layout().basis.scale;
				entry.instance.transform.position = entry.t.cooker.position + util::random_range(-dxy, dxy);
				entry.t.vibrateRemain = vibrate;
			}
			entry.t.vibrateRemain -= dt;
			entry.t.cooker.ready -= dt;
			entry.instance.tint = vf::Rgba::make(0xffffccff).linear();
			entry.t.text.set_string(util::format_elapsed(entry.t.cooker.ready));
		}
	}

	m_sfx.gain(game()->sfx_gain() * 0.25f);
	switch (m_sfx.state()) {
	case capo::State::ePlaying: {
		if (!ticking) { m_sfx.stop(); }
		break;
	}
	default: {
		if (ticking) { m_sfx.play(); }
		break;
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
