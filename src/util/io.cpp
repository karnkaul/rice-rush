#include <util/io.hpp>
#include <util/logger.hpp>
#include <physfs.h>
#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>

namespace rr {
namespace {
namespace fs = std::filesystem;

std::vector<std::string> g_prefixes{};

bool read_zip(std::vector<std::byte>& out, char const* uri) {
	if (PHYSFS_exists(uri) == 0) { return false; }
	auto file = PHYSFS_openRead(uri);
	if (!file) { return false; }
	auto const offset = out.size();
	auto const length = PHYSFS_fileLength(file);
	out.resize(out.size() + static_cast<std::size_t>(length));
	PHYSFS_readBytes(file, out.data() + offset, static_cast<PHYSFS_uint64>(length));
	return true;
}

bool read_file(std::vector<std::byte>& out, char const* uri) {
	for (auto const& prefix : g_prefixes) {
		if (auto in = std::ifstream((fs::path(prefix) / uri), std::ios::binary | std::ios::ate)) {
			in.unsetf(std::ios::skipws);
			auto const size = in.tellg();
			auto const offset = out.size();
			out.resize(out.size() + static_cast<std::size_t>(size));
			in.seekg(0, std::ios::beg);
			in.read(reinterpret_cast<char*>(out.data() + offset), size);
			return true;
		}
	}
	return false;
}
} // namespace

io::Instance::Instance(char const* arg0) {
	if (PHYSFS_init(arg0) != 0) {
		logger::debug("[zipfs] initialized");
	} else {
		logger::warn("[zipfs] Failed to initialize");
	}
}

io::Instance::~Instance() {
	PHYSFS_deinit();
	logger::debug("[zipfs] deinitialized");
	g_prefixes.clear();
}

bool io::mount_dir(char const* path) {
	auto const fs_path = fs::absolute(path);
	if (!fs::is_directory(path)) {
		logger::warn("[fs] Cannot mount: [{}], not a directory", path);
		return false;
	}
	auto path_str = fs_path.generic_string();
	if (std::ranges::find(g_prefixes, path_str) != std::ranges::end(g_prefixes)) {
		logger::warn("[fs] Already mounted: [{}]", path);
		return false;
	}
	logger::info("[fs] Mounted [{}]", path);
	g_prefixes.push_back(std::move(path_str));
	return true;
}

bool io::mount_zip(char const* zip, char const* point) {
	if (PHYSFS_mount(zip, point, 1) != 0) {
		logger::info("[zipfs] Mounted [{}]", zip);
		return true;
	}
	logger::warn("[zipfs] Failed to mount: [{}]", zip);
	return false;
}

bool io::exists(char const* uri) {
	if (PHYSFS_exists(uri) != 0) { return true; }
	for (auto const& prefix : g_prefixes) {
		auto const path = fs::path(prefix) / uri;
		if (fs::is_regular_file(path)) { return true; }
	}
	return false;
}

bool io::load(std::vector<std::byte>& out, char const* uri) {
	if (read_zip(out, uri)) {
		logger::debug("[zipfs] loaded [{}]", uri);
		return true;
	}

	if (read_file(out, uri)) {
		logger::debug("[fs] loaded [{}]", uri);
		return true;
	}

	logger::warn("[fs] Failed to open [{}]", uri);
	return false;
}
} // namespace rr
