#include <engine/context.hpp>
#include <game/game.hpp>
#include <game/hud.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <util/util.hpp>

namespace rr {
void Hud::setup() {
	layer = layers::hud;
	auto& vfc = game()->context.vfContext;
	m_health = {vfc, "health"};
	m_score = {vfc, "score"};

	auto& resources = game()->resources();
	auto& hud = layout().hud;

	m_background = vf::Mesh(vfc, "hud_background");
	m_background.gbo.write(vf::Geometry::makeQuad({hud.extent}));
	m_background.instance.transform.position = hud.offset;
	m_background.instance.tint = bgColour;

	m_score.transform().position = layout().hud.offset;
	m_score.setFont(&resources.fonts.main).setHeight(static_cast<vf::Text::Height>(hud.extent.y * 0.6f));

	auto his = healthIconSize * basis().scale;
	auto healthX = hud.offset.x + 0.5f * (-hud.extent.x + his) + his;
	for (int i = 0; i < game()->player().maxHp; ++i) {
		m_health.instances.push_back(vf::DrawInstance{vf::Transform{{healthX, hud.offset.y}}});
		healthX += 2.0f * his;
	}
	m_health.gbo.write(vf::Geometry::makeQuad(vf::QuadCreateInfo{{his, his}}));

	if constexpr (debug_v) {
		m_debug = {vfc, "debug"};
		m_debug.transform().position = {hud.offset.x + 0.5f * hud.extent.x - layout().basis.scale * 50.0f, hud.offset.y};
		m_debug.setFont(&resources.fonts.main).setHeight(static_cast<vf::Text::Height>(hud.extent.y * 0.2f)).setAlign({vf::Text::Horz::eRight});
	}
}

void Hud::tick(DeltaTime dt) {
	auto const score = game()->player().score();
	if (score == 0) {
		m_score.setString("--");
	} else {
		m_score.setString(ktl::kformat("{}", game()->player().score()));
	}

	auto const& health = game()->player().health();
	auto hp = static_cast<std::size_t>(std::clamp(health.hp, 0, static_cast<int>(m_health.instances.size())));
	if (health.isImmune()) {
		m_hurtElapsed += dt.real;
		if (m_hurtElapsed >= hurtPulse) {
			m_hurtElapsed = {};
			m_hurtPulseUp = !m_hurtPulseUp;
		}
	} else {
		m_hurtElapsed = {};
		m_hurtPulseUp = false;
	}
	if (m_hurtPulseUp) { ++hp; }
	for (std::size_t i = 0; i < hp; ++i) {
		m_health.instances[i].tint.channels[3] = 0xff;
		m_health.instances[i].transform.scale = {1.0f, 1.0f};
	}
	for (std::size_t i = hp; i < m_health.instances.size(); ++i) {
		m_health.instances[i].tint.channels[3] = healthLostAlpha;
		m_health.instances[i].transform.scale = {healthLostScale, healthLostScale};
	}

	if constexpr (debug_v) { m_debug.setString(ktl::kformat("{} FPS\t{}", game()->framerate().fps(), util::formatElapsed(game()->elapsed()))); }
}

void Hud::draw(vf::Frame const& frame) const {
	frame.draw(m_background);
	frame.draw(m_health);
	frame.draw(m_score);
	if constexpr (debug_v) { frame.draw(m_debug); }
}
} // namespace rr
