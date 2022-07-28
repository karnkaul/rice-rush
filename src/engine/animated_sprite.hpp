#pragma once
#include <engine/sprite.hpp>
#include <util/sequenced_index.hpp>

namespace rr {
class AnimatedSprite : public Sprite {
  public:
	AnimatedSprite() = default;
	AnimatedSprite(Context const& context, std::string name = "animated_sprite") : Sprite(context, std::move(name)) {}

	SequencedIndex index{};

	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, SequencedIndex::Sequence sequence);
	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, vf::Time duration);

	void tick(vf::Time dt);
};
} // namespace rr
