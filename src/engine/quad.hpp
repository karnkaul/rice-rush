#pragma once
#include <vulkify/graphics/primitives/mesh.hpp>

namespace vf {
class Frame;
}

namespace rr {
class Context;

class Quad {
  public:
	struct Info {
		vf::QuadCreateInfo createInfo{};
		vf::TextureHandle texture{};
	};

	Quad() = default;
	explicit Quad(Context const& context, Info const& info, std::string name = "quad");

	Quad& setSize(glm::vec2 size);
	Quad& setUv(vf::UvRect uv);
	Quad& setTexture(vf::TextureHandle const& texture);

	vf::DrawInstance& instance() { return m_mesh.instance; }
	vf::DrawInstance const& instance() const { return m_mesh.instance; }
	Info const& info() const { return m_info; }
	glm::vec2 size() const { return m_info.createInfo.size; }
	vf::UvRect uv() const { return m_info.createInfo.uv; }
	vf::TextureHandle const& texture() const { return m_info.texture; }

	void draw(vf::Frame const& frame) const;

  private:
	Quad& refresh();

	vf::Mesh m_mesh{};
	Info m_info{};
};
} // namespace rr
