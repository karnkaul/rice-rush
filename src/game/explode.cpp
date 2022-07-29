#include <game/explode.hpp>
#include <game/game.hpp>
#include <game/player.hpp>

namespace rr {
void Explode::tick(DeltaTime dt) {
	m_elapsed += dt.scaled;
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
