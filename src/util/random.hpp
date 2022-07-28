#pragma once
#include <glm/vec2.hpp>
#include <random>

namespace rr::util {
inline std::default_random_engine& randomEngine() {
	static auto eng = std::default_random_engine(std::random_device{}());
	return eng;
}

inline float randomRange(float min, float max) { return std::uniform_real_distribution<float>(min, max)(randomEngine()); }
inline glm::vec2 randomRange(glm::vec2 min, glm::vec2 max) { return {randomRange(min.x, max.x), randomRange(min.y, max.y)}; }

template <std::integral T>
T randomRange(T min, T max) {
	return std::uniform_int_distribution<T>(min, max)(randomEngine());
}
} // namespace rr::util
