#pragma once
#include <engine/quad.hpp>
#include <util/ptr.hpp>
#include <vulkify/graphics/resources/texture.hpp>

namespace rr {
class Sprite {
  public:
	class Sheet;

	Sprite() = default;
	explicit Sprite(Context const& context, std::string name = "sprite");

	Sprite& setSize(glm::vec2 size);
	Sprite& setUvIndex(std::size_t index);
	Sprite& setSheet(Ptr<Sheet const> sheet);

	Quad const& quad() const { return m_quad; }
	vf::DrawInstance& instance() { return m_quad.instance(); }
	vf::DrawInstance const& instance() const { return m_quad.instance(); }

	void draw(vf::Frame const& frame) const { m_quad.draw(frame); }

  private:
	Quad m_quad{};
	Ptr<Sheet const> m_sheet{};
};

class Sprite::Sheet {
  public:
	Sheet& setTexture(vf::Texture texture);
	vf::Texture const& texture() const { return m_texture; }

	std::size_t addUv(vf::UvRect uv);
	vf::UvRect const& uv(std::size_t index) const;

	Sheet& setUvs(std::size_t rows = 1, std::size_t columns = 1, glm::uvec2 pad = {});

  private:
	vf::Texture m_texture{};
	std::vector<vf::UvRect> m_uvs{};
};
} // namespace rr
