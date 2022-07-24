#pragma once
#include <vulkify/core/time.hpp>

namespace rr {
struct DeltaTime {
	vf::Time real{};
	vf::Time scaled{};
};
} // namespace rr
