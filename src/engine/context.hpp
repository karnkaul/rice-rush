#pragma once
#include <capo/capo.hpp>
#include <vulkify/vulkify.hpp>

#include <engine/audio.hpp>
#include <engine/basis.hpp>
#include <engine/config.hpp>
#include <engine/env.hpp>

namespace rr {
struct Context {
	Env env{};
	ktl::kunique_ptr<capo::Instance> capo_instance{};
	vf::Context vf_context;
	Config::Scoped config{};

	Audio audio{};
	Basis basis{};

	constexpr float basis_scale(glm::vec2 framebuffer_size) const {
		auto const x = framebuffer_size.x / basis.space.x;
		auto const y = framebuffer_size.y / basis.space.y;
		return std::min(x, y);
	}
};
} // namespace rr
