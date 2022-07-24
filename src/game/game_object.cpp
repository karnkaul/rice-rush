#include <game/game.hpp>
#include <game/game_object.hpp>

namespace rr {
Layout const& GameObject::layout() const { return m_game->layout; }
Basis const& GameObject::basis() const { return layout().basis; }
float GameObject::timeScale() const { return m_game->timeScale; }
} // namespace rr
