#include <engine/context.hpp>
#include <game/background.hpp>
#include <game/game.hpp>
#include <game/resources.hpp>
#include <algorithm>

namespace rr {
Background::Background(vf::Context const& vf_context) : m_vf_context(&vf_context) {}

void Background::set_texture(vf::Texture const& texture, Layout const& layout, std::uint32_t columns) {
	if (!texture) { return; }
	columns = std::clamp(columns, 1U, 256U);
	auto const width = layout.play_area.extent.x / static_cast<float>(columns);
	auto const textureExtent = glm::vec2(texture.extent());
	auto const height = width * textureExtent.y / textureExtent.x;
	auto const rows = static_cast<std::uint32_t>(layout.play_area.extent.y / height) + 1;
	auto xy = 0.5f * glm::vec2(-layout.play_area.extent.x + width, layout.play_area.extent.y - height);
	xy += layout.play_area.offset;
	auto const left = xy.x;
	auto geometry = vf::Geometry{};
	for (std::uint32_t i = 0; i < rows; ++i) {
		xy.x = left;
		for (std::uint32_t i = 0; i < columns; ++i) {
			geometry.add_quad(vf::QuadCreateInfo{{width, height}, {xy}});
			xy.x += width;
		}
		xy.y -= height;
	}

	m_mesh = vf::Mesh{*m_vf_context, texture.handle()};
	m_mesh.buffer.write(std::move(geometry));
}

void Background::draw(vf::Frame const& frame) const { frame.draw(m_mesh); }
} // namespace rr
