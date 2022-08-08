#include <engine/animated_sprite.hpp>
#include <engine/context.hpp>

namespace rr {
AnimatedSprite::AnimatedSprite(Context const& context) : Sprite(context.vf_context) { draw_invalid = true; }

AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, Sequence sequence) {
	Sprite::set_sheet(&sheet);
	m_sheet = &sheet;
	timeline.set(sequence);
	return *this;
}

AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, vf::Time const duration) { return set_sheet(sheet, {duration, sheet.uv_count()}); }

void AnimatedSprite::tick(vf::Time dt) {
	timeline.tick(dt);
	if (m_sheet) { Sprite::set_sheet(m_sheet, static_cast<UvIndex>(timeline.index())); }
}

AnimatedSprite& AnimatedSprite::unset_sheet() {
	Sprite::set_sheet({});
	timeline = {};
	return *this;
}
} // namespace rr
