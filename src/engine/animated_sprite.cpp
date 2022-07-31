#include <engine/animated_sprite.hpp>

namespace rr {
AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, Sequence sequence) {
	Sprite::set_sheet(&sheet);
	timeline.set(sequence);
	return *this;
}

AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, vf::Time const duration) { return set_sheet(sheet, {duration, sheet.uv_count()}); }

void AnimatedSprite::tick(vf::Time dt) {
	timeline.tick(dt);
	set_uv_index(timeline.index());
}

AnimatedSprite& AnimatedSprite::unset_sheet() {
	Sprite::set_sheet({});
	timeline = {};
	return *this;
}
} // namespace rr
