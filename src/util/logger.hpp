#pragma once
#include <ktl/kformat.hpp>

namespace logger {
enum class Pipe { StdOut, StdErr };

std::string logString(std::string_view text);
void doPrint(Pipe pipe, char level, char const* text);

template <typename... Args>
void print(Pipe pipe, char level, std::string_view const fmt, Args const&... args) {
	doPrint(pipe, level, logString(ktl::kformat(fmt, args...)).c_str());
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
} // namespace logger
