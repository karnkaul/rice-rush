#pragma once
#include <capo/source.hpp>
#include <engine/instanced.hpp>
#include <engine/trigger.hpp>
#include <game/game_object.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace rr {
class ExplodePool;

class CookerPool : public GameObject {
  public:
	using Points = std::uint32_t;

	struct Cooker {
		glm::vec2 position{};
		vf::Time cook{5s};
		vf::Time ready{3s};
		Points points{10};
	};

	struct Prefab {
		glm::vec2 size{100.0f};
		vf::TextureHandle texture{};
	};

	void set_prefab(Prefab prefab);
	void spawn(Cooker const& cooker);
	std::size_t sweep_ready();

	Prefab const& prefab() const { return m_prefab; }
	bool intersecting(Trigger const& trigger) const;

	vf::Text::Height textHeight{36};
	float triggerDiameter{150.0f};
	vf::Time vibrate{0.1s};
	vf::Rgba text_tint{vf::white_v};

  private:
	struct Entry {
		vf::Text text{};

		Trigger trigger{};
		Cooker cooker{};
		vf::Time vibrateRemain{};
	};

	void setup() override;
	void tick(DeltaTime dt) override;
	void draw(vf::Frame const& frame) const override;
	void add_triggers(std::vector<Ptr<Trigger const>>& out) const override;

	Ptr<Instanced<Entry>::Entry> update_and_get_nearest(Trigger& player, vf::Time dt);
	void spawn_collect(glm::vec2 position) const;
	void pop(Instanced<Entry>::Entry& entry);

	Instanced<Entry> m_entries{};
	Prefab m_prefab{};
	capo::Source m_sfx{};
	Ptr<ExplodePool> m_explode_pool{};
};
} // namespace rr
