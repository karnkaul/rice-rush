#include <engine/context.hpp>
#include <game/game.hpp>
#include <game/hud.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <util/logger.hpp>
#include <util/util.hpp>

namespace rr {
void Hud::setup() {
	layer = layers::hud;
	auto& vfc = game()->context.vf_context;
	m_health = {vfc};
	m_score = vf::Text{vfc};
	m_over.title = vf::Text{vfc};
	m_over.high_score = vf::Text{vfc};
	m_over.restart = vf::Text{vfc};

	auto& resources = game()->resources;
	auto& hud = layout().hud;

	m_background = {vfc};
	m_background.buffer.write(vf::Geometry::make_quad({hud.extent}));
	m_background.instance().transform.position = hud.offset;
	m_background.instance().tint = bg_tint;

	m_score.transform().position = layout().hud.offset;
	m_score.set_font(&resources.fonts.main).set_height(static_cast<vf::Text::Height>(hud.extent.y * 0.6f));

	auto his = health_icon_size * basis().scale;
	auto healthX = hud.offset.x + 0.5f * (-hud.extent.x + his) + his;
	for (int i = 0; i < game()->player().max_hp; ++i) {
		m_health.storage[i].transform.position = {healthX, hud.offset.y};
		healthX += 2.0f * his;
	}
	m_health.buffer.write(vf::Geometry::make_quad(vf::QuadCreateInfo{{his, his}}));
	m_health.texture = game()->resources.textures.health.handle();

	m_over.title.set_font(&resources.fonts.main).set_height(static_cast<vf::Text::Height>(layout().play_area.extent.y * 0.2f));
	m_over.high_score.set_font(&resources.fonts.main).set_height(static_cast<vf::Text::Height>(layout().play_area.extent.y * 0.1f));
	m_over.restart.set_font(&resources.fonts.main).set_height(static_cast<vf::Text::Height>(layout().play_area.extent.y * 0.05f));
	m_over.title.transform().position.y = layout().play_area.offset.y;
	m_over.high_score.transform().position.y = layout().play_area.offset.y + 0.5f * layout().play_area.extent.y - 100.0f * basis().scale;
	m_over.restart.transform().position.y = (0.5f * -basis().space.y + 50.0f) * basis().scale;
	m_over.high_score.set_string(ktl::kformat("HIGH SCORE\n{}", game()->high_score()));
	m_over.title.set_string("GAME OVER");
	m_over.restart.set_string("press [Enter / Start] to retry");

	if constexpr (logger::debug_v) {
		m_debug = vf::Text{vfc};
		m_debug.transform().position = {hud.offset.x + 0.5f * hud.extent.x - layout().basis.scale * 50.0f, hud.offset.y};
		m_debug.set_font(&resources.fonts.main).set_height(static_cast<vf::Text::Height>(hud.extent.y * 0.2f)).set_align({vf::Text::Horz::eRight});
	}
}

void Hud::tick(DeltaTime dt) {
	auto const score = game()->player().score();
	if (score == 0) {
		m_score.set_string("--");
	} else {
		m_score.set_string(ktl::kformat("{}", game()->player().score()));
	}

	auto const& health = game()->player().health();
	auto hp = static_cast<std::size_t>(std::clamp(health.hp, 0, static_cast<int>(std::size(m_health.storage))));
	if (health.is_immune()) {
		m_hurt_elapsed += dt.real;
		if (m_hurt_elapsed >= hurt_pulse) {
			m_hurt_elapsed = {};
			m_hurt_pulse_up = !m_hurt_pulse_up;
		}
	} else {
		m_hurt_elapsed = {};
		m_hurt_pulse_up = false;
	}
	if (m_hurt_pulse_up) { ++hp; }
	for (std::size_t i = 0; i < hp; ++i) {
		m_health.storage[i].tint.channels[3] = 0xff;
		m_health.storage[i].transform.scale = {1.0f, 1.0f};
	}
	for (std::size_t i = hp; i < std::size(m_health.storage); ++i) {
		m_health.storage[i].tint.channels[3] = health_lost_alpha;
		m_health.storage[i].transform.scale = {health_lost_scale, health_lost_scale};
	}

	if (auto high_score = game()->high_score(); m_high_score != high_score) {
		m_over.high_score.set_string(ktl::kformat("HIGH SCORE\n{}", high_score));
		m_high_score = high_score;
	}

	if constexpr (logger::debug_v) { m_debug.set_string(ktl::kformat("{} FPS\t{}", game()->framerate().fps(), util::format_elapsed(game()->elapsed()))); }
}

void Hud::draw(vf::Frame const& frame) const {
	if (game()->state() == Game::State::eOver) {
		frame.draw(m_over.title);
		frame.draw(m_over.restart);
		frame.draw(m_over.high_score);
	}

	frame.draw(m_background);
	frame.draw(m_health);
	frame.draw(m_score);
	if constexpr (logger::debug_v) { frame.draw(m_debug); }
}
} // namespace rr
