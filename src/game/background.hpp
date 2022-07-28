#pragma once
#include <game/game_object.hpp>

namespace rr {
class Background : public GameObject {
  public:
	void setTexture(vf::Texture const& texture, std::uint32_t columns = 8);

  private:
	void setup() override { layer = layers::background; }
	void tick(DeltaTime) override {}
	void draw(vf::Frame const& frame) const override;

	vf::Mesh m_mesh{};
};
} // namespace rr
