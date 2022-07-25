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

	auto const size = basis().space * glm::vec2(1.0f, layout().nPadY * 0.5f);
	auto const y = 0.5f * (basis().space.y - size.y);

	auto& resources = game()->resources();
	m_score.transform().position.y = y;
	m_score.setFont(&resources.fonts.main).setHeight(static_cast<vf::Text::Height>(size.y * 0.5f));

	auto his = healthIconSize * basis().scale;
	auto healthX = 0.5f * (-size.x + his) + his;
	for (int i = 0; i < game()->player().maxHp; ++i) {
		m_health.instances.push_back(vf::DrawInstance{vf::Transform{{healthX, y}}});
		healthX += 2.0f * his;
	}
	m_health.gbo.write(vf::Geometry::makeQuad(vf::QuadCreateInfo{{his, his}}));

	if constexpr (debug_v) {
		m_debug = {vfc, "debug"};
		m_debug.transform().position = {0.5f * size.x - 50.0f * basis().scale, y};
		m_debug.setFont(&resources.fonts.main).setHeight(static_cast<vf::Text::Height>(size.y * 0.3f)).setAlign({vf::Text::Horz::eRight});
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
	frame.draw(m_health);
	frame.draw(m_score);
	if constexpr (debug_v) { frame.draw(m_debug); }
}
} // namespace rr
