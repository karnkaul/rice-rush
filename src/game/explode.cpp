#include <engine/audio.hpp>
#include <game/explode.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>

#include <engine/context.hpp>

namespace rr {
void Explode::setup() {
	auto const& anim = game()->resources.animations.explode;
	auto sequence = anim.sequence;
	sequence.duration = force.duration;
	sprite.set_sheet(anim.sheet, anim.sequence);
	game()->audio().play(game()->resources.sfx.explode);
}

void Explode::tick(DeltaTime dt) {
	m_elapsed += dt.scaled;
	sprite.tick(dt.scaled);
	game()->time_scale = timeScale;
	if (m_elapsed >= force.duration) {
		destroy();
		game()->time_scale = 1.0f;
		return;
	}

	auto& player = game()->player();
	auto const dir = glm::normalize(player.sprite.instance().transform.position - sprite.instance().transform.position);
	player.translate(force.impulse * dt.scaled.count() * basis().scale * dir);
}
} // namespace rr
