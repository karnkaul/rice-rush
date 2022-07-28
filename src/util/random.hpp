#pragma once
#include <glm/vec2.hpp>
#include <random>

namespace rr::util {
inline std::default_random_engine& random_engine() {
	static auto eng = std::default_random_engine(std::random_device{}());
	return eng;
}

inline float random_range(float min, float max) { return std::uniform_real_distribution<float>(min, max)(random_engine()); }
inline glm::vec2 random_range(glm::vec2 min, glm::vec2 max) { return {random_range(min.x, max.x), random_range(min.y, max.y)}; }

template <std::integral T>
T randomRange(T min, T max) {
	return std::uniform_int_distribution<T>(min, max)(random_engine());
}
} // namespace rr::util
