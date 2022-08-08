#pragma once
#include <glm/vec2.hpp>
#include <vulkify/instance/instance_enums.hpp>
#include <string>

namespace rr {
struct Config {
	vf::AntiAliasing antiAliasing{vf::AntiAliasing::eNone};
	vf::VSync vsync{vf::VSync::eOn};
	glm::uvec2 extent{1280, 720};
	float sfx_gain{1.0f};
	float music_gain{1.0f};

	static Config load(std::string_view exe_dir, char const* uri, bool create_if_absent = true);
};
} // namespace rr
