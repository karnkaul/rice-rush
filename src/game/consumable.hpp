#pragma once
#include <engine/animated_sprite.hpp>
#include <engine/trigger.hpp>
#include <game/game_object.hpp>

namespace rr {
class Consumable : public GameObject {
  public:
	AnimatedSprite sprite{};
	Trigger trigger{};
	vf::Time ttl{2s};

	virtual void consume() = 0;

  protected:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override { sprite.draw(frame); }
};
} // namespace rr
