#pragma once
#include <engine/trigger.hpp>
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace rr {
class CookerPool : public GameObject {
  public:
	using Points = std::uint32_t;

	void spawn(glm::vec2 position, vf::Time ttl, Points points = 10);

	vf::Text::Height textHeight{30};
	float triggerDiameter{200.0f};

  private:
	struct Entry {
		Sprite sprite{};
		vf::Text text{};

		Trigger trigger{};
		vf::Time ttl{};
		Points points{};
	};

	void setup() override { layer = 10; }
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;
	void addTriggers(std::vector<Ptr<Trigger const>>& out) const override;

	std::vector<Entry>::iterator updateTriggersAndGetNearest(Trigger& player, vf::Time dt);
	void spawnCollect(glm::vec2 position) const;

	std::vector<Entry> m_entries{};
};
} // namespace rr
