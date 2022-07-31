#pragma once
#include <glm/vec2.hpp>
#include <vulkify/core/time.hpp>
#include <cassert>
#include <random>
#include <span>

namespace rr::util {
inline std::default_random_engine& random_engine() {
	static auto eng = std::default_random_engine(std::random_device{}());
	return eng;
}

inline float random_range(float min, float max) { return std::uniform_real_distribution<float>(min, max)(random_engine()); }
inline vf::Time random_range(vf::Time min, vf::Time max) { return vf::Time(random_range(min.count(), max.count())); }
inline glm::vec2 random_range(glm::vec2 min, glm::vec2 max) { return {random_range(min.x, max.x), random_range(min.y, max.y)}; }

template <std::integral T>
T random_range(T min, T max) {
	return std::uniform_int_distribution<T>(min, max)(random_engine());
}

template <typename T>
T const& weighted_select(std::span<T const> items, std::span<std::uint32_t const> weights) {
	assert(!items.empty() && items.size() == weights.size());
	auto total = std::uint32_t{};
	for (auto const& weight : weights) {
		assert(weight > 0);
		total += weight;
	}
	auto const norm = random_range(0.0f, 1.0f);
	auto index = static_cast<std::int64_t>(norm * static_cast<float>(total));
	for (std::size_t i = 0; i < items.size(); ++i) {
		index -= static_cast<std::int64_t>(weights[i]);
		if (index <= 0) { return items[i]; }
	}
	assert(false && "invariant violated");
	return items[0];
}
} // namespace rr::util
