#include <engine/config.hpp>
#include <util/logger.hpp>
#include <fstream>

namespace rr {
namespace {
std::string trim(std::string_view str) {
	while (!str.empty() && std::isspace(static_cast<unsigned char>(str.front()))) { str = str.substr(1); }
	while (!str.empty() && std::isspace(static_cast<unsigned char>(str.back()))) { str = str.substr(0, str.size() - 1); }
	return std::string(str);
}

std::pair<std::string, std::string> get_property(std::ifstream& file) {
	auto line = std::string{};
	std::getline(file, line);
	auto const eq = line.find('=');
	if (eq == std::string::npos) { return {}; }
	return {trim(std::string_view(line.data(), eq)), trim(std::string_view(line.data() + eq + 1, line.size() - eq - 1))};
}

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

void populate(Config& out, std::string_view key, std::string value) {
	if (key == "name") {
		out.playerName = std::move(value);
	} else if (key == "aa" || key == "anti_aliasing") {
		out.antiAliasing = aa(value);
	} else if (key == "width") {
		out.extent.x = static_cast<std::uint32_t>(std::atoi(value.c_str()));
	} else if (key == "height") {
		out.extent.y = static_cast<std::uint32_t>(std::atoi(value.c_str()));
	}
}
} // namespace

Config Config::Scoped::load(const char* path) {
	auto ret = Config{};
	auto file = std::ifstream(path);
	if (!file) { return ret; }
	while (file) {
		auto [key, value] = get_property(file);
		if (key.empty()) { continue; }
		populate(ret, key, std::move(value));
	}
	logger::info("[Config] loaded from [{}]", path);
	return ret;
}

bool Config::Scoped::save(Config const& config, char const* path) {
	auto file = std::ofstream(path);
	if (!file) { return false; }
	file << "name = " << config.playerName << '\n';
	file << "aa = " << aa(config.antiAliasing) << '\n';
	file << "width = " << config.extent.x << "\nheight = " << config.extent.y << '\n';
	logger::info("[Config] saved to [{}]", path);
	return true;
}
} // namespace rr
