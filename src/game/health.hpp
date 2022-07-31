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
			if (--hp > 0) { make_immune(); }
			return true;
		}
		return false;
	}

	constexpr Health& heal(int delta, int max_hp) {
		hp += delta;
		if (hp > max_hp) { hp = max_hp; }
		make_normal();
		return *this;
	}

	constexpr Health& make_immune() {
		m_state = State::eImmune;
		m_immune_for = immune;
		return *this;
	}

	constexpr Health& make_normal() {
		m_state = State::eNormal;
		return *this;
	}

	constexpr Health& tick(vf::Time const dt) {
		if ((m_immune_for > 0s) && (m_immune_for -= dt) <= 0s) { make_normal(); }
		return *this;
	}

	constexpr bool is_immune() const { return m_state == State::eImmune; }

  private:
	enum class State { eNormal, eImmune };

	State m_state{State::eNormal};
	vf::Time m_immune_for{};
};
} // namespace rr
