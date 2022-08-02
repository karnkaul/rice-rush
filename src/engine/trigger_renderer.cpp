#include <engine/context.hpp>
#include <engine/trigger_renderer.hpp>

namespace rr {
namespace {
void setup(Trigger const& trigger, vf::CircleShape& out) {
	out.transform().position = trigger.centre;
	out.tint() = trigger.interactable ? vf::red_v : vf::green_v;
	out.tint().channels[3] = 0x77;
	out.set_state({.diameter = trigger.diameter});
}
} // namespace

void TriggerRenderer::render(std::span<Ptr<Trigger const> const> triggers, vf::Frame const& frame) {
	auto circles = make_group(triggers.size());
	for (std::size_t i = 0; i < circles.size(); ++i) {
		setup(*triggers[i], circles[i]);
		frame.draw(circles[i]);
	}
}

std::span<vf::CircleShape> TriggerRenderer::make_group(std::size_t count) {
	m_circles.reserve(count);
	for (std::size_t i = m_circles.size(); i < count; ++i) { m_circles.push_back(vf::CircleShape(m_context->vf_context, "trigger")); }
	return std::span(m_circles.data(), count);
}
} // namespace rr
