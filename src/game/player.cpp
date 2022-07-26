#include <game/game.hpp>
#include <game/player.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>

namespace rr {
namespace {
template <typename T>
Ptr<T> refresh(std::span<ktl::kunique_ptr<T> const> ts, Trigger& player) {
	struct {
		Ptr<T> t{};
		float sqrDist{};
	} nearest{};

	auto setNearest = [&player, &nearest](T& pawn) {
		auto const toT = glm::length2(pawn.sprite.instance().transform.position - player.centre);
		if (!nearest.t || toT < nearest.sqrDist) { nearest = {&pawn, toT}; }
	};

	player.interactable = false;
	for (auto& t : ts) {
		t->trigger.interactable = false;
		if (player.intersecting(t->trigger)) { setNearest(*t); }
	}

	if (nearest.t) { player.interactable = nearest.t->trigger.interactable = true; }
	return nearest.t;
}

constexpr glm::vec2 clamp(glm::vec2 in, vf::Rect const& playArea, glm::vec2 const pad) {
	auto const bl = playArea.bottomLeft() + pad;
	auto const tr = playArea.topRight() - pad;
	in.x = std::clamp(in.x, bl.x, tr.x);
	in.y = std::clamp(in.y, bl.y, tr.y);
	return in;
}
} // namespace

void Player::translate(glm::vec2 xy) {
	auto& pos = sprite.instance().transform.position;
	pos += xy;
	trigger.centre = pos = clamp(pos, game()->layout.playArea, sprite.quad().size() * 0.5f);
}

void Player::collect(std::uint32_t points, bool incrementMultiplier) {
	auto const dp = points * m_state.multiplier;
	m_state.score += dp;
	if (incrementMultiplier) { ++m_state.multiplier; }
	logger::info("[Player] collected {} points, X: {}", dp, m_state.multiplier);
}

void Player::hit() {
	if (m_state.health.hit()) {
		m_state.multiplier = std::max(m_state.multiplier / 2, 1U);
		logger::info("[Player] hit; hp: {}, X: {}", m_state.health.hp, m_state.multiplier);
	}
}

void Player::heal(int hp) {
	m_state.health.hp = std::min(m_state.health.hp + hp, maxHp);
	logger::info("[Player] heal: {}; hp: {}", hp, m_state.health.hp);
}

void Player::reset(glm::vec2 const position) {
	logger::info("[Player] reset");
	m_state = {};
	sprite.instance().transform.position = trigger.centre = position;
}

void Player::tick(DeltaTime dt) {
	m_state.health.tick(dt.scaled);

	auto const cs = controller.update(game()->keyboard());
	translate(cs.xy * speed * basis().scale * dt.scaled.count());

	m_state.interact = cs.flags.test(Controller::Flag::eInteract);
}
} // namespace rr
