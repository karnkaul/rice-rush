#include <engine/context.hpp>
#include <engine/quad.hpp>
#include <vulkify/context/frame.hpp>

namespace rr {
Quad::Quad(Context const& context, Info const& info, std::string name)
	: m_mesh(vf::Mesh::makeQuad(context.vfContext, std::move(name), info.createInfo, info.texture)), m_info(info) {}

Quad& Quad::setSize(glm::vec2 size) {
	m_info.createInfo.size = size;
	return refresh();
}

Quad& Quad::setUv(vf::UvRect uv) {
	m_info.createInfo.uv = uv;
	return refresh();
}

Quad& Quad::setTexture(vf::TextureHandle const& texture) {
	m_mesh.texture = texture;
	return *this;
}

Quad& Quad::refresh() {
	m_mesh.gbo.write(vf::Geometry::makeQuad(m_info.createInfo));
	return *this;
}

void Quad::draw(vf::Frame const& frame) const { frame.draw(m_mesh); }
} // namespace rr
