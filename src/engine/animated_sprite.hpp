#pragma once
#include <engine/sprite.hpp>
#include <util/index_timeline.hpp>

namespace rr {
class AnimatedSprite : public Sprite {
  public:
	AnimatedSprite() = default;
	AnimatedSprite(Context const& context, std::string name = "animated_sprite") : Sprite(context, std::move(name)) {}

	IndexTimeline timeline{};

	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, IndexTimeline::Sequence sequence);
	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, vf::Time duration);
	AnimatedSprite& unset_sheet();

	void tick(vf::Time dt);
};
} // namespace rr
