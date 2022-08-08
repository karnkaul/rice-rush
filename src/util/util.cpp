#include <util/util.hpp>
#include <iomanip>
#include <sstream>

namespace rr {
std::string util::format_elapsed(vf::Time elapsed) {
	auto const h = std::chrono::duration_cast<std::chrono::hours>(elapsed);
	auto const m = std::chrono::duration_cast<std::chrono::minutes>(elapsed) - h;
	auto const s = std::chrono::duration_cast<std::chrono::seconds>(elapsed) - h - m;
	auto str = std::stringstream{};
	str << std::setfill('0');
	if (h > decltype(h){}) {
		str << h.count();
		str << ':';
	}
	if (m > decltype(m){}) {
		str << std::setw(2) << m.count() << ':' << std::setw(2) << s.count();
	} else {
		auto const ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed) - h - m - s;
		str << s.count() << '.' << ms.count() / 100;
	}
	return str.str();
}
} // namespace rr
