#include <engine/context.hpp>
#include <game/resources.hpp>
#include <util/logger.hpp>

namespace rr {
void Resources::Loader::operator()(vf::Ttf& out, std::string_view uri) const {
	if (!out) { out = vf::Ttf(context.vf_context, std::string(uri)); }
	if (out.load(data_path(context.env, uri).c_str())) {
		logger::info("[Resources] Ttf [{}] loaded", uri);
	} else {
		logger::warn("[Resources] Failed to load Ttf [{}]", uri);
	}
}

void Resources::Loader::operator()(vf::Texture& out, std::string_view uri, Ptr<vf::TextureCreateInfo const> info) const {
	auto image = vf::Image{};
	if (!image.load(data_path(context.env, uri).c_str())) {
		logger::warn("[Resources] Failed to open image [{}]", uri);
		return;
	}
	if (!out || info) {
		auto tci = info ? *info : vf::TextureCreateInfo{};
		out = vf::Texture(context.vf_context, std::string(uri), image, tci);
		if (!out) {
			logger::warn("[Resources] Failed to create Texture [{}]", uri);
			return;
		}
	}
	if (!out.create(image)) {
		logger::warn("[Resources] Failed to create Texture [{}]", uri);
		return;
	}
	logger::info("[Resources] Texture [{}] loaded", uri);
}
} // namespace rr
