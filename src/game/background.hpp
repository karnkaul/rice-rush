#pragma once
#include <util/ptr.hpp>
#include <vulkify/context/frame.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/resources/texture.hpp>

namespace rr {
class Game;
struct Layout;

class Background {
  public:
	Background(Game& game);

	void set_texture(vf::Texture const& texture, std::uint32_t columns = 8);
	void draw(vf::Frame const& frame) const;

  private:
	Ptr<Game> game() const { return m_game; }
	Layout const& layout() const;

	Ptr<Game> m_game{};
	vf::Mesh m_mesh{};
};
} // namespace rr
