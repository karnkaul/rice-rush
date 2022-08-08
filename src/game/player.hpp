#pragma once
#include <engine/animated_sprite.hpp>
#include <engine/trigger.hpp>
#include <game/controller.hpp>
#include <game/game_object.hpp>
#include <game/health.hpp>

namespace rr {
class Pawn;

class Player : public GameObject {
  public:
	AnimatedSprite sprite{};
	Controller controller{};
	Trigger trigger{};

	glm::vec2 size{150.0f};
	float speed{500.0f};
	int max_hp{3};
	vf::Time anim_duration{0.3s};

	void translate(glm::vec2 xy);
	void collect(std::uint32_t points, bool increment_multiplier = true);
	void hit();
	void heal(int hp);
	bool interact() const { return m_state.flags.test(Controller::Flag::eInteract); }
	bool start() const { return m_state.flags.test(Controller::Flag::eStart); }

	void reset(glm::vec2 position = {});

	std::uint64_t score() const { return m_state.score; }
	Health const& health() const { return m_state.health; }

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override { frame.draw(sprite); }

	struct {
		Health health{};
		std::uint64_t score{};
		std::uint32_t multiplier{1};
		Controller::Flags flags{};
	} m_state{};
};
} // namespace rr
