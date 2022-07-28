#pragma once
#include <util/ptr.hpp>
#include <vulkify/graphics/resources/texture.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace rr {
struct Context;

struct Resources {
	struct {
		vf::Ttf main{};
	} fonts{};

	struct {
		vf::Texture background{};
	} textures{};

	struct Loader;
};

struct Resources::Loader {
	Context& context;

	void operator()(vf::Ttf& out, std::string_view uri) const;
	void operator()(vf::Texture& out, std::string_view uri, Ptr<vf::TextureCreateInfo const> info = {}) const;
};
} // namespace rr
