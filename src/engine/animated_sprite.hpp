#pragma once
#include <util/index_timeline.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace rr {
struct Context;

class AnimatedSprite : public vf::Sprite {
  public:
	using Sequence = IndexTimeline::Sequence;

	AnimatedSprite() = default;
	AnimatedSprite(Context const& context);

	IndexTimeline timeline{};

	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, Sequence sequence);
	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, vf::Time duration);
	AnimatedSprite& unset_sheet();

	void tick(vf::Time dt);

  private:
	Ptr<Sprite::Sheet const> m_sheet{};
};
} // namespace rr
