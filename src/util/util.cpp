#include <engine/context.hpp>
#include <util/logger.hpp>
#include <util/util.hpp>
#include <vulkify/graphics/resources/texture.hpp>
#include <iomanip>
#include <sstream>

namespace rr {
std::string util::formatElapsed(vf::Time elapsed) {
	auto const h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
	auto const m = std::chrono::duration_cast<std::chrono::minutes>(elapsed) - h;
	auto const s = std::chrono::duration_cast<std::chrono::seconds>(elapsed) - h - m;
	auto str = std::stringstream{};
	str << std::setfill('0');
	if (h > decltype(h){}) {
		str << h.count();
		str << ':';
	}
	if (m > decltype(m){}) {
		str << std::setw(2) << m.count() << ':' << std::setw(2) << s.count();
	} else {
		auto const ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed) - h - m - s;
		str << s.count() << '.' << std::setw(2) << ms.count() / 10;
	}
	return str.str();
}

vf::Texture util::makeTexture(Context const& context, std::string_view uri) {
	auto image = vf::Image{};
	if (!image.load(rr::dataPath(context.env, "textures/awesomeface.png").c_str())) {
		logger::error("[Resources] Failed to load Image [{}]", uri);
		return {};
	}
	return vf::Texture(context.vfContext, std::string(uri), image);
}
} // namespace rr
