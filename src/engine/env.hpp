#pragma once
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace rr {
inline constexpr bool debug_v =
#if defined(RR_DEBUG)
	true;
#else
	false;
#endif

struct Env {
	struct {
		std::string exe{};
		std::string pwd{};
		std::string data{};
	} paths{};
	std::span<char const* const> argv{};

	static Env make(int argc, char const* const argv[]);
};

template <typename T>
struct TArray {
	std::unique_ptr<T[]> t{};
	std::size_t size{};
};

std::string dataPath(Env const& env, std::string_view uri);
std::vector<std::string> fileList(Env const& env, std::string_view dir, std::string_view ext);
} // namespace rr
