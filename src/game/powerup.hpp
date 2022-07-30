#pragma once
#include <engine/sprite.hpp>
#include <engine/trigger.hpp>
#include <game/game_object.hpp>

namespace rr {
class Powerup : public GameObject {
  public:
	using Modify = void (*)(Game&);

	struct Request {
		Modify modify{};
		Ptr<Sprite::Sheet const> sheet{};
		std::size_t uv_index{0};
		vf::Time ttl{2s};
		float diameter{100.0f};
	};

	glm::vec2 random_position(float distance_factor = 0.2f, int max_tries = 20) const;
	bool active() const { return m_request.modify != nullptr; }
	bool activate(Request request, glm::vec2 position);
	void deactivate();

	float squish_coeff{0.05f};
	float squish_rate{10.0f};

  protected:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;
	void add_triggers(std::vector<Ptr<Trigger const>>& out) const override;

	Sprite m_sprite{};
	Request m_request{};
	Trigger m_trigger{};
	vf::Time m_elapsed{};
};
} // namespace rr
