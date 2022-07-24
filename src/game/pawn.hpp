#pragma once
#include <engine/trigger.hpp>
#include <game/game_object.hpp>

namespace rr {
class Player;

class Pawn : public GameObject {
  public:
	Trigger trigger{};

  private:
	void addTriggers(std::vector<Ptr<Trigger const>>& out) const override { out.push_back(&trigger); }
};
} // namespace rr
