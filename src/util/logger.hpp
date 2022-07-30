#pragma once
#include <ktl/kformat.hpp>

namespace logger {
enum class Pipe { StdOut, StdErr };

constexpr bool debug_v =
#if defined RR_DEBUG
	true;
#else
	false;
#endif

std::string log_string(std::string_view text);
void do_print(Pipe pipe, char level, char const* text);

template <typename... Args>
void print(Pipe pipe, char level, std::string_view const fmt, Args const&... args) {
	do_print(pipe, level, log_string(ktl::kformat(fmt, args...)).c_str());
}

template <typename... Args>
void error(std::string_view const fmt, Args const&... args) {
	print(Pipe::StdErr, 'E', fmt, args...);
}

template <typename... Args>
void warn(std::string_view const fmt, Args const&... args) {
	print(Pipe::StdOut, 'W', fmt, args...);
}

template <typename... Args>
void info(std::string_view const fmt, Args const&... args) {
	print(Pipe::StdOut, 'I', fmt, args...);
}

template <typename... Args>
void debug(std::string_view const fmt, Args const&... args) {
	if constexpr (debug_v) { print(Pipe::StdOut, 'D', fmt, args...); }
}
} // namespace logger
