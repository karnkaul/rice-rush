#pragma once
#include <capo/instance.hpp>

namespace rr {
class Audio {
  public:
	Audio() = default;
	Audio(capo::Instance& instance) { m_sfx = instance.makeSource(); }

	Audio& set_sfx_gain(float value) {
		m_sfx.gain(value);
		return *this;
	}

	float sfx_gain() const { return m_sfx.gain(); }

	Audio& play(capo::Sound const& sound) {
		m_sfx.bind(sound);
		m_sfx.play();
		return *this;
	}

  private:
	capo::Source m_sfx{};
};
} // namespace rr
