#pragma once
#include <glm/vec2.hpp>
#include <vulkify/instance/instance_enums.hpp>
#include <string>

namespace rr {
struct Config {
	struct Scoped;

	std::string playerName{"Bowl"};
	vf::AntiAliasing antiAliasing{vf::AntiAliasing::eNone};
	glm::uvec2 extent{1280, 720};
};

struct Config::Scoped {
  public:
	Scoped() = default;
	Scoped(std::string path) : config(load(path.c_str())), path(std::move(path)) {}
	Scoped(Scoped&&) = default;
	Scoped& operator=(Scoped&&) = default;
	~Scoped() {
		if (!path.empty()) { save(config, path.c_str()); }
	}

	static Config load(char const* path);
	static bool save(Config const& config, char const* path);

	Config config{};
	std::string path{};
};
} // namespace rr
