#pragma once
#include <engine/basis.hpp>
#include <vulkify/core/rect.hpp>

namespace rr {
struct Layout {
	Basis basis{};
	vf::Rect playArea{};
	vf::Rect hud{};
	float nPadY{0.1f};
};

namespace layers {
inline constexpr int player{10};
inline constexpr int consumable{20};
inline constexpr int hud{50};
} // namespace layers
} // namespace rr
