#pragma once
#include <engine/trigger.hpp>
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace rr {
class CookerPool : public GameObject {
  public:
	using Points = std::uint32_t;

	struct Cooker {
		glm::vec2 position{};
		vf::Time cook{5s};
		vf::Time ready{3s};
		Points points{10};
	};

	void spawn(Cooker const& cooker);

	vf::Text::Height textHeight{30};
	float triggerDiameter{200.0f};
	vf::Time vibrate{0.1s};

  private:
	struct Entry {
		Sprite sprite{};
		vf::Text text{};

		Trigger trigger{};
		Cooker cooker{};
		vf::Time vibrateRemain{};
	};

	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;
	void addTriggers(std::vector<Ptr<Trigger const>>& out) const override;

	std::vector<Entry>::iterator updateTriggersAndGetNearest(Trigger& player, vf::Time dt);
	void spawnCollect(glm::vec2 position) const;
	void pop(Entry& entry);

	std::vector<Entry> m_entries{};
};
} // namespace rr
