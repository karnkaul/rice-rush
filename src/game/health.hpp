#pragma once
#include <vulkify/core/time.hpp>

namespace rr {
using namespace std::chrono_literals;

class Health {
  public:
	vf::Time immune{2s};
	int hp{3};

	constexpr bool hit() {
		if (m_state == State::eNormal && hp > 0) {
			if (--hp > 0) { makeImmune(); }
			return true;
		}
		return false;
	}

	constexpr Health& makeImmune() {
		m_state = State::eImmune;
		m_immuneFor = immune;
		return *this;
	}

	constexpr Health& makeNormal() {
		m_state = State::eNormal;
		return *this;
	}

	constexpr Health& tick(vf::Time const dt) {
		if ((m_immuneFor > 0s) && (m_immuneFor -= dt) <= 0s) { makeNormal(); }
		return *this;
	}

  private:
	enum class State { eNormal, eImmune };

	State m_state{State::eNormal};
	vf::Time m_immuneFor{};
};
} // namespace rr
