#pragma once
#include <engine/animated_sprite.hpp>
#include <engine/trigger.hpp>
#include <game/game_object.hpp>

namespace rr {
class Powerup : public GameObject {
  public:
	using Modify = void (*)(Game&);

	struct Request {
		Modify modify{};
		Ptr<Sprite::Sheet const> sheet{};
		IndexTimeline::Sequence sequence{};
		vf::Time ttl{2s};
		float diameter{50.0f};
	};

	glm::vec2 random_position(float distance_factor = 0.2f, int max_tries = 20) const;
	bool active() const { return m_request.modify != nullptr; }
	bool activate(Request request, glm::vec2 position);
	void deactivate();

  protected:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;
	void add_triggers(std::vector<Ptr<Trigger const>>& out) const override;

	AnimatedSprite m_sprite{};
	Request m_request{};
	Trigger m_trigger{};
};
} // namespace rr
