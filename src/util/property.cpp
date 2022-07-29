#include <util/property.hpp>

namespace rr::util {
std::string trim(std::string_view str) {
	while (!str.empty() && std::isspace(static_cast<unsigned char>(str.front()))) { str = str.substr(1); }
	while (!str.empty() && std::isspace(static_cast<unsigned char>(str.back()))) { str = str.substr(0, str.size() - 1); }
	return std::string(str);
}

Property Property::Parser::next() {
	if (!in) { return {}; }
	auto line = std::string{};
	std::getline(in, line);
	if (line.front() == '#') { return next(); }
	auto const eq = line.find('=');
	if (eq == std::string::npos) { return next(); }
	return {trim(std::string_view(line.data(), eq)), trim(std::string_view(line.data() + eq + 1, line.size() - eq - 1))};
}
} // namespace rr::util
