#include <engine/env.hpp>
#include <filesystem>

namespace rr {
namespace stdfs = std::filesystem;

namespace {
stdfs::path find_data(stdfs::path start) {
	while (!start.empty() && start.parent_path() != start) {
		auto ret = start / "data";
		if (stdfs::is_directory(ret)) { return ret; }
		start = start.parent_path();
	}
	return {};
}
} // namespace

Env Env::make(int argc, char const* const* argv) {
	auto ret = Env{};
	ret.argv = {argv, static_cast<std::size_t>(argc)};
	ret.paths.pwd = stdfs::absolute(stdfs::current_path()).generic_string();
	if (!ret.argv.empty()) { ret.paths.exe = stdfs::absolute(ret.argv[0]).generic_string(); }
	ret.paths.data = find_data(ret.paths.exe.empty() ? ret.paths.pwd : ret.paths.exe).generic_string();
	if (ret.paths.data.empty()) { ret.paths.data = ret.paths.pwd; }
	return ret;
}
} // namespace rr

std::string rr::data_path(Env const& env, std::string_view uri) { return (stdfs::path(env.paths.data) / uri).generic_string(); }
std::string rr::exe_path(Env const& env, std::string_view uri) { return (stdfs::path(env.paths.exe).parent_path() / uri).generic_string(); }

std::vector<std::string> rr::file_list(Env const& env, std::string_view dir, std::string_view ext) {
	auto ret = std::vector<std::string>{};
	for (auto const& it : stdfs::directory_iterator(data_path(env, dir))) {
		if (it.is_regular_file() && it.path().extension() == ext) { ret.push_back(it.path().generic_string()); }
	}
	return ret;
}
