#pragma once
#include <game/game_object.hpp>

namespace rr {
class Explode : public GameObject {
  public:
	struct Force {
		float impulse{1000.0f};
		vf::Time duration{0.1s};
	};

	Force force{};
	float timeScale{0.75f};

  private:
	void tick(DeltaTime dt) override;

	vf::Time m_elapsed{};
};
} // namespace rr
