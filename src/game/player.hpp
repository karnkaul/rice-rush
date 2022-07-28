#pragma once
#include <engine/trigger.hpp>
#include <game/controller.hpp>
#include <game/game_object.hpp>
#include <game/health.hpp>

namespace rr {
class Pawn;

class Player : public GameObject {
  public:
	Controller controller{};
	std::string name{};
	Trigger trigger{};
	float speed{500.0f};
	int max_hp{3};

	void translate(glm::vec2 xy);
	void collect(std::uint32_t points, bool increment_multiplier = true);
	void hit();
	void heal(int hp);
	bool interact() const { return m_state.interact; }

	void reset(glm::vec2 position = {});

	std::uint64_t score() const { return m_state.score; }
	Health const& health() const { return m_state.health; }

  private:
	void setup() override { layer = layers::player; }
	void tick(DeltaTime dt) override;

	struct {
		Health health{};
		std::uint64_t score{};
		std::uint32_t multiplier{1};
		bool interact{};
	} m_state{};
};
} // namespace rr
