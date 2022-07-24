#pragma once
#include <engine/trigger.hpp>
#include <game/pawn.hpp>

namespace rr {
class Consumable : public Pawn {
  public:
	vf::Time ttl{2s};

	virtual void consume() = 0;

  protected:
	void tick(DeltaTime dt) override;
};
} // namespace rr
