#pragma once
#include <capo/utils/enum_array.hpp>
#include <game/game_object.hpp>

namespace rr {
enum class PowerupType { eSweep, eSlomo, eHeal, eCOUNT_ };

class Director : public GameObject {
	struct Wave {
		std::pair<vf::Time, vf::Time> spawn_delay{1s, 3s};
		std::uint32_t pressure{2};
		vf::Time ttl{10s};
	};

	struct {
		capo::utils::EnumArray<PowerupType, std::uint32_t> weights{5U, 3U, 2U};
		std::pair<vf::Time, vf::Time> respawn{10s, 60s};
		float slowmo_scale{0.5f};
	} m_powerup{};

	static vf::Time decrement(vf::Time current, vf::Time dt, vf::Time min = 0.2s);

	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const&) const override {}

	void spawn_powerup() const;
	void next_wave();
	void begin_wave();
	void tick_wave(vf::Time dt);

	Wave m_wave{};
	Wave m_active{};
	vf::Time m_next_powerup{};
	vf::Time m_next_spawn{};
	vf::Time m_next_cooker_wave{};
};
} // namespace rr
