#pragma once
#include <capo/sound.hpp>
#include <engine/sprite.hpp>
#include <util/index_timeline.hpp>
#include <util/ptr.hpp>
#include <vulkify/core/time.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace rr {
struct Context;

struct SheetAnimation {
	using Sequence = IndexTimeline::Sequence;

	vf::Texture texture{};
	Sprite::Sheet sheet{};
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

	bool operator()(vf::Ttf& out, std::string_view uri) const;
	bool operator()(vf::Texture& out, std::string_view uri, Ptr<vf::TextureCreateInfo const> info = {}) const;
	bool operator()(SheetAnimation& out_anim, std::string_view uri) const;
	bool operator()(capo::Sound& out, std::string_view uri) const;

	bool operator()(Resources& out, std::string_view uri) const;
};
} // namespace rr
