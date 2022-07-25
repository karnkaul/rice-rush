#pragma once
#include <engine/basis.hpp>

namespace rr {
struct Layout {
	Basis basis{};
	glm::vec2 playArea{};
	float nPadY{0.2f};
};

namespace layers {
inline constexpr int player{10};
inline constexpr int consumable{20};
inline constexpr int hud{50};
} // namespace layers
} // namespace rr
