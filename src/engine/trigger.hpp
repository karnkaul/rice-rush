#pragma once
#include <glm/vec2.hpp>

namespace rr {
struct Trigger {
	glm::vec2 centre{};
	float diameter{150.0f};
	bool interactable{};

	constexpr bool intersecting(Trigger const& other) const {
		auto const dd = (other.diameter + diameter) * 0.5f;
		auto const dc = other.centre - centre;
		auto const sqm = dc.x * dc.x + dc.y * dc.y;
		return sqm < dd * dd;
	}
};
} // namespace rr
