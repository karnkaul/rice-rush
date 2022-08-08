#pragma once
#include <engine/trigger.hpp>
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>
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

	struct {
		vf::Time spawn{5s};
		vf::Time effect{5s};
	} ttl{};
	float squish_coeff{0.05f};
	float squish_rate{10.0f};
	float diameter{50.0f};
	std::uint32_t points{20};

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

	vf::Sprite::Sheet m_sheet{};
	vf::Sprite m_sprite{};
	Trigger m_trigger{};
	struct {
		ktl::kunique_ptr<Modifier> modifier{};
		vf::Time elapsed{};
		bool effect{};
	} m_active{};
};
} // namespace rr
