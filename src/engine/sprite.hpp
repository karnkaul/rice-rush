#pragma once
#include <util/ptr.hpp>
#include <vulkify/graphics/primitives/quad_shape.hpp>

namespace vf {
class Frame;
} // namespace vf

namespace rr {
class Context;

class Sprite {
  public:
	class Sheet;

	Sprite() = default;
	explicit Sprite(Context const& context);

	Sprite& set_size(glm::vec2 size);
	Sprite& set_uv_index(std::size_t index);
	Sprite& set_sheet(Ptr<Sheet const> sheet);

	vf::QuadShape const& quad() const { return m_quad; }
	vf::Transform const& transform() const { return m_quad.transform(); }
	vf::Transform& transform() { return m_quad.transform(); }
	vf::Rgba const& tint() const { return m_quad.tint(); }
	vf::Rgba& tint() { return m_quad.tint(); }
	Ptr<Sheet const> sheet() const { return m_sheet; }

	void draw(vf::Frame const& frame) const;

  private:
	vf::QuadShape m_quad{};
	Ptr<Sheet const> m_sheet{};
};

class Sprite::Sheet {
  public:
	Sheet() = default;
	Sheet(Ptr<vf::Texture const> texture) : m_texture(texture) {}

	Sheet& set_texture(Ptr<vf::Texture const> texture);
	Ptr<vf::Texture const> const& texture() const { return m_texture; }

	std::size_t add_uv(vf::UvRect uv);
	vf::UvRect const& uv(std::size_t index) const;
	std::size_t uv_count() const { return m_uvs.size(); }

	Sheet& set_uvs(std::size_t rows = 1, std::size_t columns = 1, glm::uvec2 pad = {});

  private:
	std::vector<vf::UvRect> m_uvs{};
	Ptr<vf::Texture const> m_texture{};
};
} // namespace rr
