#pragma once
#include <engine/quad.hpp>
#include <util/ptr.hpp>
#include <vulkify/graphics/resources/texture.hpp>

namespace rr {
class Sprite {
  public:
	class Sheet;
	struct Sequence;

	Sprite() = default;
	explicit Sprite(Context const& context, std::string name = "sprite");

	Sprite& set_size(glm::vec2 size);
	Sprite& set_uv_index(std::size_t index);
	Sprite& set_sheet(Ptr<Sheet const> sheet);

	Quad const& quad() const { return m_quad; }
	vf::DrawInstance& instance() { return m_quad.instance(); }
	vf::DrawInstance const& instance() const { return m_quad.instance(); }
	Ptr<Sheet const> sheet() const { return m_sheet; }

	void draw(vf::Frame const& frame) const { m_quad.draw(frame); }

  private:
	Quad m_quad{};
	Ptr<Sheet const> m_sheet{};
};

class Sprite::Sheet {
  public:
	Sheet() = default;
	Sheet(vf::Texture texture) { set_texture(std::move(texture)); }

	Sheet& set_texture(vf::Texture texture);
	vf::Texture const& texture() const { return m_texture; }

	std::size_t add_uv(vf::UvRect uv);
	vf::UvRect const& uv(std::size_t index) const;
	std::size_t uv_count() const { return m_uvs.size(); }

	Sheet& set_uvs(std::size_t rows = 1, std::size_t columns = 1, glm::uvec2 pad = {});

  private:
	vf::Texture m_texture{};
	std::vector<vf::UvRect> m_uvs{};
};
} // namespace rr
