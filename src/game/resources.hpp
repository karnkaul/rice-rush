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
	Sprite::Sheet sheet{};
	IndexTimeline::Sequence sequence{};
};

struct Resources {
	struct {
		vf::Ttf main{};
	} fonts{};

	struct {
		vf::Texture background{};
		vf::Texture health{};
		vf::Texture cooker{};
	} textures{};

	struct {
		SheetAnimation explode{};
		SheetAnimation player{};
	} animations{};

	struct {
		capo::Sound explode{};
	} sfx{};

	struct Loader;
};

struct Resources::Loader {
	Context& context;

	bool operator()(vf::Ttf& out, std::string_view uri) const;
	bool operator()(vf::Texture& out, std::string_view uri, Ptr<vf::TextureCreateInfo const> info = {}) const;

	bool operator()(Sprite::Sheet& out, std::string_view uri) const;
	bool operator()(SheetAnimation& out_anim, std::string_view uri) const;

	bool operator()(capo::Sound& out, std::string_view uri) const;
};
} // namespace rr
