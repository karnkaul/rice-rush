#pragma once
#include <engine/delta_time.hpp>
#include <engine/sprite.hpp>
#include <game/layout.hpp>

namespace rr {
class Game;
struct Trigger;

using namespace std::chrono_literals;

class GameObject {
  public:
	GameObject() = default;
	GameObject(GameObject&&) = default;
	GameObject& operator=(GameObject&&) = default;
	virtual ~GameObject() = default;

	void destroy() { m_destroyed = true; }
	bool destroyed() const { return m_destroyed; }

	Sprite sprite{};
	int layer{};

  protected:
	virtual void setup() {}
	virtual void tick(DeltaTime dt) = 0;
	virtual void draw(vf::Frame const& frame) const { sprite.draw(frame); }
	virtual void add_triggers(std::vector<Ptr<Trigger const>>&) const {}

	Ptr<Game> game() const { return m_game; }
	Layout const& layout() const;
	Basis const& basis() const;
	float time_scale() const;

  private:
	friend class Game;

	Ptr<Game> m_game{};
	bool m_destroyed{};
};
} // namespace rr
