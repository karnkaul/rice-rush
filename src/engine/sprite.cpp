#include <engine/context.hpp>
#include <engine/sprite.hpp>
#include <vulkify/context/frame.hpp>
#include <vulkify/graphics/texture.hpp>

namespace rr {
Sprite::Sprite(Context const& context) : m_quad(context.vf_context) {}

Sprite& Sprite::set_size(glm::vec2 size) {
	auto state = m_quad.state();
	state.size = size;
	m_quad.set_state(state);
	return *this;
}

Sprite& Sprite::set_uv_index(std::size_t index) {
	if (m_sheet) {
		auto state = m_quad.state();
		state.uv = m_sheet->uv(index);
		m_quad.set_state(state);
	}
	return *this;
}

Sprite& Sprite::set_sheet(Ptr<Sheet const> sheet) {
	m_sheet = sheet;
	if (auto texture = m_sheet ? m_sheet->texture() : nullptr) {
		m_quad.set_texture(texture);
		set_uv_index(0);
	} else {
		m_quad.set_texture({});
	}
	return *this;
}

void Sprite::draw(vf::Frame const& frame) const { frame.draw(m_quad); }

auto Sprite::Sheet::set_texture(Ptr<vf::Texture const> texture) -> Sheet& {
	m_texture = texture;
	return *this;
}

std::size_t Sprite::Sheet::add_uv(vf::UvRect uv) {
	auto const ret = m_uvs.size();
	m_uvs.push_back(uv);
	return ret;
}

vf::UvRect const& Sprite::Sheet::uv(std::size_t index) const {
	static constexpr auto default_v = vf::UvRect{};
	if (index >= m_uvs.size()) { return default_v; }
	return m_uvs[index];
}

auto Sprite::Sheet::set_uvs(std::size_t const rows, std::size_t const columns, glm::uvec2 const pad) -> Sheet& {
	if (!m_texture || !*m_texture || rows == 0 || columns == 0) { return *this; }
	m_uvs.clear();
	auto const extent = glm::vec2(m_texture->extent());
	auto const frame = glm::vec2(extent.x / static_cast<float>(columns), extent.y / static_cast<float>(rows));
	auto const padf = glm::vec2(pad);
	auto const tile = frame - 2.0f * padf;
	auto o = glm::vec2{};
	for (std::size_t row = 0; row < rows; ++row) {
		o.x = {};
		for (std::size_t column = 0; column < columns; ++column) {
			auto const tl = o + padf;
			auto const br = tl + tile - padf;
			add_uv(vf::UvRect{.top_left = tl / extent, .bottom_right = br / extent});
			o.x += frame.x;
		}
		o.y += frame.y;
	}
	return *this;
}
} // namespace rr
