#include <engine/animated_sprite.hpp>

namespace rr {
AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, SequencedIndex::Sequence sequence) {
	Sprite::set_sheet(&sheet);
	index.set(sequence);
	return *this;
}

AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, vf::Time const duration) { return set_sheet(sheet, {duration, sheet.uv_count()}); }

void AnimatedSprite::tick(vf::Time dt) {
	index.tick(dt);
	set_uv_index(index.index());
}
} // namespace rr
