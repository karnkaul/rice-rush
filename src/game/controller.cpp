#include <engine/keyboard.hpp>
#include <game/controller.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <vulkify/instance/gamepad.hpp>

namespace rr {
namespace {
constexpr float debounce(float const axis, float const epsilon = 0.1f) { return std::abs(axis) > epsilon ? axis : 0.0f; }

constexpr bool non_empty(Controller::State const& state) { return state.xy.x != 0.0f || state.xy.y != 0.0f || state.flags.any(); }

using Flag = Controller::Flag;

constexpr void update(Controller::Flags& out_state, Controller::Flags& out_cache, Controller::Flag const flag, bool set) {
	if (set) {
		if (!out_cache.test(flag)) { out_state.set(flag); }
		out_cache.set(flag);
	} else {
		out_cache.reset(flag);
	}
}

bool set_state(vf::Gamepad::Id const id, Controller::State& out, Controller::Flags& cache) {
	if (auto gamepad = vf::Gamepad{id}) {
		out.xy.x = debounce(gamepad(vf::GamepadAxis::eLeftX));
		out.xy.y = debounce(gamepad(vf::GamepadAxis::eLeftY));
		update(out.flags, cache, Flag::eInteract, gamepad(vf::GamepadButton::eA));
		return non_empty(out);
	}
	return false;
}

bool set_state(Keyboard const& keyboard, Controller::State& out, Controller::Flags& cache) {
	if (keyboard.pressed(vf::Key::eW) || keyboard.pressed(vf::Key::eUp)) { out.xy.y += 1.0f; }
	if (keyboard.pressed(vf::Key::eS) || keyboard.pressed(vf::Key::eDown)) { out.xy.y -= 1.0f; }
	if (keyboard.pressed(vf::Key::eD) || keyboard.pressed(vf::Key::eRight)) { out.xy.x += 1.0f; }
	if (keyboard.pressed(vf::Key::eA) || keyboard.pressed(vf::Key::eLeft)) { out.xy.x -= 1.0f; }
	update(out.flags, cache, Flag::eInteract, keyboard.pressed(vf::Key::eSpace));
	if (out.xy.x != 0.0f || out.xy.y != 0.0f) { out.xy = glm::normalize(out.xy); }
	return non_empty(out);
}
} // namespace

Controller::State Controller::update(Keyboard const& keyboard) {
	auto ret = State{};
	auto const map = vf::Gamepad::map();
	for (auto id = vf::Gamepad::Id{}; id < vf::Gamepad::max_id_v; ++id) {
		if (map.map[id] && set_state(id, ret, m_cache)) {
			if (m_gamepadId != id) { logger::info("Controller changed: Gamepad_{}", m_gamepadId); }
			m_gamepadId = id;
			m_type = Type::eGamepad;
			return ret;
		}
	}
	if (set_state(keyboard, ret, m_cache)) {
		if (m_type != Type::eKeyboard) { logger::info("Controller changed: Keyboard"); }
		m_type = Type::eKeyboard;
	}
	return ret;
}
} // namespace rr
