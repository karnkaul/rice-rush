#include <engine/context.hpp>
#include <game/resources.hpp>
#include <util/logger.hpp>

namespace rr {
void Resources::Loader::operator()(vf::Ttf& out, std::string_view uri) const {
	if (!out) { out = vf::Ttf(context.vfContext, std::string(uri)); }
	if (out.load(dataPath(context.env, uri).c_str())) {
		logger::info("[Resources] Ttf [{}] loaded", uri);
	} else {
		logger::warn("[Resources] Failed to load Ttf [{}]", uri);
	}
}
} // namespace rr
