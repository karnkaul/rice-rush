#include <engine/context.hpp>
#include <game/resources.hpp>
#include <util/io.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <fstream>

namespace rr {
namespace {
struct SheetInfo {
	std::string image_uri{};
	glm::ivec2 tile_count{};
};

SheetInfo get_sheet_info(std::istream& in, Ptr<SheetAnimation::Sequence> sequence = {}) {
	auto parser = util::Property::Parser{in};
	auto ret = SheetInfo{};
	parser.parse_all([&](util::Property property) {
		if (property.key == "image") {
			ret.image_uri = std::move(property.value);
		} else if (property.key == "x") {
			ret.tile_count.x = std::atoi(property.value.c_str());
		} else if (property.key == "y") {
			ret.tile_count.y = std::atoi(property.value.c_str());
		} else if (property.key == "duration" && sequence) {
			sequence->duration = vf::Time(std::atof(property.value.c_str()));
		} else if (sequence && property.key == "begin") {
			auto begin = std::atoi(property.value.c_str());
			if (begin >= 0) { sequence->begin = static_cast<std::size_t>(begin); }
		} else if (sequence && property.key == "end") {
			auto end = std::atoi(property.value.c_str());
			if (end >= 0) { sequence->end = static_cast<std::size_t>(end); }
		}
	});
	return ret;
}

bool validate(SheetInfo const& info, std::string_view const uri) {
	if (info.image_uri.empty()) {
		logger::warn("[Resources] Required field missing/empty in Sprite::Sheet [{}]: [image]", uri);
		return false;
	}
	if (info.tile_count.x <= 0 || info.tile_count.y <= 0) {
		logger::warn("[Resources] Required field missing/empty in Sprite::Sheet [{}]: [x/y]", uri);
		return false;
	}
	return true;
}

bool validate(IndexTimeline::Sequence const& sequence, std::string_view const uri) {
	if (sequence.duration < 0s) {
		logger::warn("[Resources] Invalid duration in Sprite::Sheet [{}]: [{}]", uri, sequence.duration.count());
		return false;
	}
	if (sequence.begin > sequence.end) {
		logger::warn("[Resources] Invalid sequence in Sprite::Sheet [{}]: [{}] - [{}]", sequence.begin, sequence.end);
		return false;
	}
	return true;
}

bool load(ktl::byte_array& out_buffer, vf::Image& out, char const* uri) {
	if (!io::load(out_buffer, uri)) {
		logger::warn("[Resources] Failed to read image: [{}]", uri);
		return false;
	}
	if (!out.load(vf::Image::Encoded{out_buffer})) {
		logger::warn("[Resources] Failed to open image: [{}]", uri);
		return false;
	}
	return true;
}
} // namespace

bool Resources::Loader::operator()(vf::Ttf& out, char const* uri) {
	if (!out) { out = vf::Ttf{context.vf_context}; }
	if (!io::load(buffer, uri)) {
		logger::warn("[Resources] Failed to read Ttf: [{}]", uri);
		return false;
	}
	if (!out.load(buffer)) {
		logger::warn("[Resources] Failed to load Ttf: [{}]", uri);
		return false;
	}
	return true;
}

bool Resources::Loader::operator()(vf::Texture& out, char const* uri, Ptr<vf::TextureCreateInfo const> info) {
	auto image = vf::Image{};
	if (!load(buffer, image, uri)) { return false; }
	if (!out || info) {
		auto tci = info ? *info : vf::TextureCreateInfo{};
		out = vf::Texture(context.vf_context, image, tci);
		if (!out) {
			logger::warn("[Resources] Failed to create Texture: [{}]", uri);
			return false;
		}
	}
	if (!out.create(image)) {
		logger::warn("[Resources] Failed to create Texture: [{}]", uri);
		return false;
	}
	return true;
}

bool Resources::Loader::operator()(SheetAnimation& out_anim, char const* uri) {
	if (!io::load(buffer, uri)) {
		logger::warn("[Resources] Failed to read sheet animation: [{}]", uri);
		return false;
	}
	auto str = std::stringstream{reinterpret_cast<char const*>(buffer.data())};
	auto const info = get_sheet_info(str, &out_anim.sequence);
	if (!validate(info, uri)) { return false; }
	if (!validate(out_anim.sequence, uri)) { return false; }

	auto image = vf::Image{};
	if (!load(buffer, image, info.image_uri.c_str())) { return false; }
	out_anim.texture = vf::Texture(context.vf_context, image);
	out_anim.sheet = &out_anim.texture;
	out_anim.sheet.set_uvs(static_cast<std::size_t>(info.tile_count.y), static_cast<std::size_t>(info.tile_count.x));
	if (out_anim.sequence.end <= out_anim.sequence.begin) { out_anim.sequence.end = out_anim.sheet.uv_count(); }

	return true;
}

bool Resources::Loader::operator()(capo::Sound& out, char const* uri) {
	if (!io::load(buffer, uri)) {
		logger::warn("[Resources] Failed to read PCM: [{}]", uri);
		return false;
	}
	auto pcm = capo::PCM::from_memory(buffer, capo::FileFormat::eUnknown);
	if (!pcm) {
		logger::warn("[Resources] Failed to load PCM: [{}]", uri);
		return false;
	}
	out = context.capo_instance->make_sound(*pcm);
	return true;
}

bool Resources::Loader::operator()(Resources& out, char const* uri) {
	if (!io::load(buffer, uri)) {
		logger::warn("[Resources] Failed to read manifest: [{}]", uri);
		return false;
	}
	auto str = std::stringstream{std::string(reinterpret_cast<char const*>(buffer.data()), buffer.size())};
	auto parser = util::Property::Parser{str};
	int count{};
	parser.parse_all([&](util::Property property) {
		if (property.key == "fonts/main") {
			if (operator()(out.fonts.main, property.value.c_str())) { ++count; }
		} else if (property.key == "textures/background") {
			if (operator()(out.textures.background, property.value.c_str())) { ++count; }
		} else if (property.key == "textures/health") {
			if (operator()(out.textures.health, property.value.c_str())) { ++count; }
		} else if (property.key == "textures/cooker") {
			if (operator()(out.textures.cooker, property.value.c_str())) { ++count; }
		} else if (property.key == "textures/powerups/heal") {
			if (operator()(out.textures.powerups.heal, property.value.c_str())) { ++count; }
		} else if (property.key == "textures/powerups/slomo") {
			if (operator()(out.textures.powerups.slomo, property.value.c_str())) { ++count; }
		} else if (property.key == "textures/powerups/sweep") {
			if (operator()(out.textures.powerups.sweep, property.value.c_str())) { ++count; }
		} else if (property.key == "animations/explode") {
			if (operator()(out.animations.explode, property.value.c_str())) { ++count; }
		} else if (property.key == "animations/player") {
			if (operator()(out.animations.player, property.value.c_str())) { ++count; }
		} else if (property.key == "sfx/tick_tock") {
			if (operator()(out.sfx.tick_tock, property.value.c_str())) { ++count; }
		} else if (property.key == "sfx/explode") {
			if (operator()(out.sfx.explode, property.value.c_str())) { ++count; }
		} else if (property.key == "sfx/collect") {
			if (operator()(out.sfx.collect, property.value.c_str())) { ++count; }
		} else if (property.key == "sfx/powerup") {
			if (operator()(out.sfx.powerup, property.value.c_str())) { ++count; }
		} else {
			logger::warn("[Resources] Unrecognized key in [{}]: [{}]", uri, property.key);
		}
	});
	logger::debug("[Resources] loaded [{}] assets from [{}]", count, uri);
	return true;
}
} // namespace rr
