#pragma once
#include <engine/trigger.hpp>
#include <util/ptr.hpp>
#include <vulkify/graphics/primitives/circle_shape.hpp>

namespace vf {
class Frame;
}

namespace rr {
struct Context;

class TriggerRenderer {
  public:
	TriggerRenderer(Context& context) : m_context(&context) {}

	void render(std::span<Ptr<Trigger const> const> triggers, vf::Frame const& frame);

  private:
	std::span<vf::CircleShape> make_group(std::size_t count);

	std::vector<vf::CircleShape> m_circles{};
	Ptr<Context> m_context{};
};
} // namespace rr
