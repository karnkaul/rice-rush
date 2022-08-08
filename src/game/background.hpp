#pragma once
#include <util/ptr.hpp>
#include <vulkify/context/frame.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/texture.hpp>

namespace rr {
struct Layout;

class Background {
  public:
	Background(vf::Context const& vf_context);

	void set_texture(vf::Texture const& texture, Layout const& layout, std::uint32_t columns = 8);
	void draw(vf::Frame const& frame) const;

  private:
	vf::Mesh m_mesh{};
	Ptr<vf::Context const> m_vf_context{};
};
} // namespace rr
