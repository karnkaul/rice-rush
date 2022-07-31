#include <game/cooker_pool.hpp>
#include <game/director.hpp>
#include <game/game.hpp>
#include <game/powerup.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>

namespace rr {
vf::Time Director::decrement(vf::Time current, vf::Time const dt, vf::Time const min) {
	current -= dt;
	if (current < min) { current = min; }
	return current;
}

void Director::setup() {
	m_next_powerup = util::random_range(m_powerup.respawn.first, m_powerup.respawn.second);
	begin_wave();
}

void Director::tick(DeltaTime dt) {
	if (game()->state() != Game::State::ePlay) { return; }

	m_next_powerup -= dt.real;

	if (m_next_powerup <= 0s) {
		spawn_powerup();
		m_next_powerup = util::random_range(m_powerup.respawn.first, m_powerup.respawn.second);
	}

	if (m_active.ttl <= 0s) {
		m_next_cooker_wave -= dt.real;
		if (m_next_cooker_wave <= 0s && game()->cooker_pool()->count() == 0) { next_wave(); }
	} else {
		tick_wave(dt.real);
	}
}

void Director::spawn_powerup() const {
	static constexpr PowerupType types_v[] = {PowerupType::eSweep, PowerupType::eSlomo, PowerupType::eHeal};
	static constexpr auto span = std::span(types_v, std::size(types_v));
	auto const& weights = m_powerup.weights.t;
	switch (util::weighted_select(span, weights)) {
	case PowerupType::eHeal: game()->powerup()->activate_heal(1); break;
	case PowerupType::eSlomo: game()->powerup()->activate_slowmo(m_powerup.slowmo_scale); break;
	case PowerupType::eSweep: game()->powerup()->activate_sweep(); break;
	default: break;
	}
}

void Director::next_wave() {
	m_wave.ttl += 5s;
	m_wave.spawn_delay.first = decrement(m_wave.spawn_delay.first, 0.5s, 0.2s);
	m_wave.spawn_delay.second = decrement(m_wave.spawn_delay.second, 1s, 0.5s);
	m_wave.pressure = std::max(m_active.pressure, m_wave.pressure + 2);
	begin_wave();
}

void Director::begin_wave() {
	m_active = m_wave;
	m_next_spawn = util::random_range(m_active.spawn_delay.first, m_active.spawn_delay.second);
	logger::debug("[Director] begun wave, ttl: {:.1f}s, pressure: {}", m_active.ttl.count(), m_active.pressure);
}

void Director::tick_wave(vf::Time dt) {
	m_active.ttl -= dt;
	m_next_spawn -= dt;

	auto const count = game()->cooker_pool()->count();
	if (m_next_spawn <= 0s && count < m_active.pressure) {
		game()->cooker_pool()->spawn();
		m_next_spawn = util::random_range(m_active.spawn_delay.first, m_active.spawn_delay.second);
	}
}
} // namespace rr
