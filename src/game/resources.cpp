#include <engine/context.hpp>
#include <game/resources.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <fstream>

namespace rr {
namespace {
struct SheetInfo {
	std::string image_uri{};
	glm::ivec2 tile_count{};
};

SheetInfo get_sprite_info(std::istream& in, Ptr<IndexTimeline::Sequence> sequence = {}) {
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
		} else if (property.key == "begin" && sequence) {
			auto begin = std::atoi(property.value.c_str());
			if (begin >= 0) { sequence->begin = static_cast<std::size_t>(begin); }
		} else if (property.key == "end" && sequence) {
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

bool load(vf::Image& out, char const* path) {
	if (!out.load(path)) {
		logger::warn("[Resources] Failed to open image: [{}]", path);
		return false;
	}
	return true;
}

bool load(Context const& context, Sprite::Sheet& out_sheet, std::string_view uri, IndexTimeline::Sequence* out_sequence) {
	auto file = std::ifstream(data_path(context.env, uri));
	if (!file) {
		logger::warn("[Resources] Failed to load Sprite::Sheet: [{}]", uri);
		return false;
	}
	auto const info = get_sprite_info(file, out_sequence);
	if (!validate(info, uri)) { return false; }
	if (out_sequence && !validate(*out_sequence, uri)) { return false; }

	auto image = vf::Image{};
	if (!load(image, data_path(context.env, info.image_uri).c_str())) { return false; }
	out_sheet.set_texture(vf::Texture(context.vf_context, std::string(uri), image));
	out_sheet.set_uvs(static_cast<std::size_t>(info.tile_count.y), static_cast<std::size_t>(info.tile_count.x));
	if (out_sequence && out_sequence->end <= out_sequence->begin) { out_sequence->end = out_sheet.uv_count(); }

	logger::info("[Resources] Sprite::Sheet [{}] loaded", uri);
	return true;
}
} // namespace

bool Resources::Loader::operator()(vf::Ttf& out, std::string_view uri) const {
	if (!out) { out = vf::Ttf(context.vf_context, std::string(uri)); }
	if (out.load(data_path(context.env, uri).c_str())) {
		logger::info("[Resources] Ttf [{}] loaded", uri);
		return true;
	} else {
		logger::warn("[Resources] Failed to load Ttf: [{}]", uri);
		return false;
	}
}

bool Resources::Loader::operator()(vf::Texture& out, std::string_view uri, Ptr<vf::TextureCreateInfo const> info) const {
	auto image = vf::Image{};
	if (!load(image, data_path(context.env, uri).c_str())) { return false; }
	if (!out || info) {
		auto tci = info ? *info : vf::TextureCreateInfo{};
		out = vf::Texture(context.vf_context, std::string(uri), image, tci);
		if (!out) {
			logger::warn("[Resources] Failed to create Texture: [{}]", uri);
			return false;
		}
	}
	if (!out.create(image)) {
		logger::warn("[Resources] Failed to create Texture: [{}]", uri);
		return false;
	}
	logger::info("[Resources] Texture [{}] loaded", uri);
	return true;
}

bool Resources::Loader::operator()(Sprite::Sheet& out_sheet, std::string_view uri) const { return load(context, out_sheet, uri, {}); }
bool Resources::Loader::operator()(SheetAnimation& out_anim, std::string_view uri) const { return load(context, out_anim.sheet, uri, &out_anim.sequence); }

bool Resources::Loader::operator()(capo::Sound& out, std::string_view uri) const {
	auto pcm = capo::PCM::fromFile(data_path(context.env, uri));
	if (!pcm) {
		logger::warn("[Resources] Failed to load PCM: [{}]", uri);
		return false;
	}
	out = context.capo_instance->makeSound(*pcm);
	logger::info("[Resources] Sound [{}] loaded", uri);
	return true;
}
} // namespace rr
