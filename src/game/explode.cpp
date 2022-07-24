#include <game/explode.hpp>
#include <game/game.hpp>
#include <game/player.hpp>

namespace rr {
void Explode::tick(DeltaTime dt) {
	m_elapsed += dt.scaled;
	game()->timeScale = timeScale;
	if (m_elapsed >= force.duration) {
		destroy();
		game()->timeScale = 1.0f;
		return;
	}

	auto& player = game()->player();
	auto const impulse = force.impulse * (force.duration - m_elapsed).count();
	auto const dir = glm::normalize(player.sprite.instance().transform.position - sprite.instance().transform.position);
	player.translate(impulse * dt.scaled.count() * basis().scale * dir);
}
} // namespace rr
