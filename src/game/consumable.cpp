#include <game/consumable.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <util/logger.hpp>

namespace rr {
void Consumable::setup() { sprite = AnimatedSprite(game()->context); }

void Consumable::tick(DeltaTime dt) {
	trigger.centre = sprite.instance().transform.position;
	if (game()->player().trigger.intersecting(trigger)) {
		consume();
		destroy();
	}
	if ((ttl -= dt.real) <= 0s) { destroy(); }
}
} // namespace rr
