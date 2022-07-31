#pragma once
#include <engine/sprite.hpp>
#include <engine/trigger.hpp>
#include <game/game_object.hpp>
#include <functional>

namespace rr {
class Powerup : public GameObject {
  public:
	glm::vec2 random_position(float distance_factor = 0.2f, int max_tries = 20) const;
	bool active() const { return m_active.modifier != nullptr; }
	void deactivate();

	bool activate_sweep();
	bool activate_slowmo(float time_scale, vf::Time duration = 2s);
	bool activate_heal(int hp);

	float diameter{50.0f};
	struct {
		vf::Time spawn{3s};
		vf::Time effect{2s};
	} ttl{};
	float squish_coeff{0.05f};
	float squish_rate{10.0f};

	struct Modifier {
		virtual ~Modifier() = default;

		virtual vf::Time enable() = 0;
		virtual void disable() {}
	};

  private:
	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;
	void add_triggers(std::vector<Ptr<Trigger const>>& out) const override;

	bool can_activate() const;
	void activate(Ptr<vf::Texture const> texture);
	void update_active(vf::Time dt);
	void update_effect(vf::Time dt);

	Sprite::Sheet m_sheet{};
	Sprite m_sprite{};
	Trigger m_trigger{};
	struct {
		ktl::kunique_ptr<Modifier> modifier{};
		vf::Time elapsed{};
		bool effect{};
	} m_active{};
};
} // namespace rr
