#pragma once
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/instanced_mesh.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace rr {
class Hud final : public GameObject {
  public:
	vf::Time hurtPulse{0.2s};
	float healthIconSize{25.0f};
	float healthLostScale{0.75f};
	std::uint8_t healthLostAlpha{0x55};
	vf::Rgba bgColour{vf::Rgba::make(0x504011ff)};

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;

	vf::Mesh m_background{};
	vf::InstancedMesh<> m_health{};
	vf::Text m_score{};
	vf::Text m_debug{};

	vf::Time m_hurtElapsed{};
	bool m_hurtPulseUp{};
};
} // namespace rr
