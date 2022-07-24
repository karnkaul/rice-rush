#pragma once
#include <vulkify/core/time.hpp>

namespace rr {
using namespace std::chrono_literals;

class Framerate {
  public:
	constexpr Framerate& tick(vf::Time dt) {
		++m_current.frames;
		m_dt = dt;
		if ((m_current.elapsed += dt) >= 1s) { update(); }
		return *this;
	}

	constexpr std::uint8_t fps() const { return m_fps == 0 ? m_current.frames : m_fps; }
	constexpr vf::Time dt() const { return m_dt; }

  private:
	constexpr void update() {
		m_fps = m_current.frames;
		m_current = {};
	}

	struct {
		std::uint8_t frames{};
		vf::Time elapsed{};
	} m_current{};
	std::uint8_t m_fps{};
	vf::Time m_dt{};
};
} // namespace rr
