#pragma once
#include <engine/basis.hpp>

namespace rr {
struct Layout {
	Basis basis{};
	glm::vec2 playArea{};
	float nPadY{0.2f};
};
} // namespace rr
