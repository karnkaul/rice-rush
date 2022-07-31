#include <engine/config.hpp>
#include <engine/env.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>

namespace rr {
namespace fs = std::filesystem;

namespace {
constexpr vf::AntiAliasing aa(std::string_view const str) {
	if (str == "16x") { return vf::AntiAliasing::e16x; }
	if (str == "8x") { return vf::AntiAliasing::e8x; }
	if (str == "4x") { return vf::AntiAliasing::e4x; }
	if (str == "2x") { return vf::AntiAliasing::e2x; }
	return vf::AntiAliasing::eNone;
}

constexpr std::string_view aa(vf::AntiAliasing const a) {
	switch (a) {
	case vf::AntiAliasing::e16x: return "16x";
	case vf::AntiAliasing::e8x: return "8x";
	case vf::AntiAliasing::e4x: return "4x";
	case vf::AntiAliasing::e2x: return "2x";
	default: return "none";
	}
}

constexpr vf::VSync vsync(std::string_view const str) {
	if (str == "off") {
		return vf::VSync::eOff;
	} else if (str == "adaptive") {
		return vf::VSync::eAdaptive;
	}
	return vf::VSync::eOn;
}

constexpr std::string_view vsync(vf::VSync const v) {
	switch (v) {
	case vf::VSync::eAdaptive: return "adaptive";
	case vf::VSync::eOff: return "off";
	default: return "on";
	}
}

void populate(Config& out, std::string_view key, std::string value) {
	if (key == "aa" || key == "anti_aliasing") {
		out.antiAliasing = aa(value);
	} else if (key == "width") {
		out.extent.x = static_cast<std::uint32_t>(std::atoi(value.c_str()));
	} else if (key == "height") {
		out.extent.y = static_cast<std::uint32_t>(std::atoi(value.c_str()));
	} else if (key == "music") {
		out.music_gain = static_cast<float>(std::atof(value.c_str()));
	} else if (key == "sfx") {
		out.sfx_gain = static_cast<float>(std::atof(value.c_str()));
	} else if (key == "vsync") {
		out.vsync = vsync(value);
	}
}

bool read(Config& out, const char* path) {
	auto file = std::ifstream(path);
	if (!file) { return false; }
	auto parser = util::Property::Parser{file};
	parser.parse_all([&](util::Property property) { populate(out, property.key, property.value); });
	return true;
}

bool write(Config const& config, char const* path) {
	auto file = std::ofstream(path);
	if (!file) { return false; }
	file << "aa = " << aa(config.antiAliasing) << '\n';
	file << "width = " << config.extent.x << "\nheight = " << config.extent.y << '\n';
	file << ktl::kformat("sfx = {:1.1f}\nmusic = {:1.1f}\n", config.sfx_gain, config.music_gain);
	file << "vsync = " << vsync(config.vsync) << '\n';
	return true;
}
} // namespace

Config Config::load(Env const& env, const char* uri, bool create_if_absent) {
	auto const path = exe_path(env, uri);
	bool const exists = fs::is_regular_file(path);
	auto ret = Config{};
	if (!exists && create_if_absent && write(ret, path.c_str())) { logger::info("[Config] Default config saved to [{}]", path); }
	if (fs::is_regular_file(uri) && read(ret, uri)) {
		logger::info("[Config] loaded overridden config [./{}]", uri);
		return ret;
	}
	if (exists && read(ret, path.c_str())) {
		logger::info("[Config] loaded config from [{}]", path);
		return ret;
	}
	return ret;
}
} // namespace rr
