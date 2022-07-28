#include <engine/sprite.hpp>

namespace rr {
Sprite::Sprite(Context const& context, std::string name) { m_quad = Quad(context, {}, std::move(name)); }

Sprite& Sprite::set_size(glm::vec2 size) {
	m_quad.set_size(size);
	return *this;
}

Sprite& Sprite::set_uv_index(std::size_t index) {
	if (m_sheet) { m_quad.set_uv(m_sheet->uv(index)); }
	return *this;
}

Sprite& Sprite::set_sheet(Ptr<Sheet const> sheet) {
	if ((m_sheet = sheet) != nullptr) {
		m_quad.set_texture(m_sheet->texture().handle()).set_uv(m_sheet->uv(0));
	} else {
		m_quad.set_texture({});
	}
	return *this;
}

auto Sprite::Sheet::set_texture(vf::Texture texture) -> Sheet& {
	m_texture = std::move(texture);
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
	if (!m_texture || rows == 0 || columns == 0) { return *this; }
	m_uvs.clear();
	auto const extent = glm::vec2(m_texture.extent());
	auto const frame = glm::vec2(extent.x / static_cast<float>(columns), extent.y / static_cast<float>(rows));
	auto const padf = glm::vec2(pad);
	auto const tile = frame - 2.0f * padf;
	auto o = glm::vec2{};
	for (std::size_t row = 0; row < rows; ++row) {
		o.x = {};
		for (std::size_t column = 0; column < columns; ++column) {
			auto const tl = o + padf;
			auto const br = tl + tile - padf;
			add_uv(vf::UvRect{.topLeft = tl / extent, .bottomRight = br / extent});
			o.x += frame.x;
		}
		o.y += frame.y;
	}
	return *this;
}
} // namespace rr
