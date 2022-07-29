#pragma once
#include <game/game_object.hpp>

namespace rr {
class ExplodePool : public GameObject {
  public:
	struct Force {
		float impulse{800.0f};
		vf::Time duration{0.1s};
	};

	Force force{};
	glm::vec2 size{100.0f};

	void spawn(glm::vec2 position);

  private:
	struct Entry {
		AnimatedSprite sprite{};
		struct {
			vf::Time force{};
			vf::Time anim{};
		} ttl{};
	};

	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;

	Entry& make_entry();

	std::vector<Entry> m_active{};
	std::vector<Entry> m_inactive{};
};
} // namespace rr
