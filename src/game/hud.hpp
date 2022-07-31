#pragma once
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/instanced_mesh.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace rr {
class Hud final : public GameObject {
  public:
	vf::Time hurt_pulse{0.2s};
	float health_icon_size{40.0f};
	float health_lost_scale{0.8f};
	std::uint8_t health_lost_alpha{0x77};
	vf::Rgba bg_tint{vf::Rgba::make(0x504011ff)};

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;

	struct {
		vf::Text title{};
		vf::Text high_score{};
		vf::Text restart{};
	} m_over{};

	vf::Mesh m_background{};
	vf::InstancedMesh<> m_health{};
	vf::Text m_score{};
	vf::Text m_debug{};

	vf::Time m_hurt_elapsed{};
	bool m_hurt_pulse_up{};
	std::uint64_t m_high_score{};
};
} // namespace rr
