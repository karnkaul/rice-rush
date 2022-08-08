#pragma once
#include <capo/sound.hpp>
#include <ktl/byte_array.hpp>
#include <util/index_timeline.hpp>
#include <util/ptr.hpp>
#include <vulkify/core/time.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace rr {
struct Context;

struct SheetAnimation {
	using Sequence = IndexTimeline::Sequence;

	vf::Texture texture{};
	vf::Sprite::Sheet sheet{};
	Sequence sequence{};
};

struct Resources {
	struct {
		vf::Ttf main{};
	} fonts{};

	struct {
		vf::Texture background{};
		vf::Texture health{};
		vf::Texture cooker{};

		struct {
			vf::Texture heal{};
			vf::Texture slomo{};
			vf::Texture sweep{};
		} powerups{};
	} textures{};

	struct {
		SheetAnimation explode{};
		SheetAnimation player{};
	} animations{};

	struct {
		capo::Sound tick_tock{};
		capo::Sound explode{};
		capo::Sound collect{};
		capo::Sound powerup{};
	} sfx{};

	struct Loader;
};

struct Resources::Loader {
	Context& context;
	ktl::byte_array buffer{};

	bool operator()(vf::Ttf& out, char const* uri);
	bool operator()(vf::Texture& out, char const* uri, Ptr<vf::TextureCreateInfo const> info = {});
	bool operator()(SheetAnimation& out_anim, char const* uri);
	bool operator()(capo::Sound& out, char const* uri);

	bool operator()(Resources& out, char const* uri);
};
} // namespace rr
