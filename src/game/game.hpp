#pragma once
#include <engine/keyboard.hpp>
#include <game/game_object.hpp>
#include <game/layout.hpp>
#include <ktl/enum_flags/enum_flags.hpp>
#include <ktl/kunique_ptr.hpp>
#include <span>

namespace vf {
class Event;
class Frame;
} // namespace vf

namespace rr {
class Context;
class Audio;
struct Resources;

class Player;
class Pawn;
class CookerPool;

class Game;

struct KeyListener {
	Ptr<Game> game{};

	virtual void operator()(vf::KeyEvent const& key) = 0;
};

class Game {
  public:
	enum class Flag { eRenderTriggers };
	using Flags = ktl::enum_flags<Flag>;

	enum class State { eIdle, ePlay, eOver };

	Layout layout{};
	float timeScale{1.0f};
	Flags flags{};
	Context& context;

	Game(Context& context);

	Game& operator=(Game&&) = delete;

	void attach(Ptr<KeyListener> listener);
	void detach(Ptr<KeyListener> listener);

	template <std::derived_from<GameObject> T, typename... Args>
	Ptr<T> spawn(glm::vec2 position, Args&&... args) {
		auto t = ktl::make_unique<T>(std::forward<Args>(args)...);
		setup(*t, position);
		auto ret = t.get();
		m_state.objects.push_back(std::move(t));
		return ret;
	}

	Keyboard const& keyboard() const;
	Resources& resources() const;
	Audio& audio() const;
	Player& player() const { return *m_player; }
	Ptr<CookerPool> cookerPool() const { return m_cookerPool; }

	void handle(std::span<vf::Event const> events);
	void tick(vf::Time dt);
	void render(vf::Frame const& frame) const;

	void set(State state);
	State state() const { return m_state.state; }

  private:
	void onKey(vf::KeyEvent const& key);
	void setup(GameObject& go, glm::vec2 position);
	template <typename T>
	static void tick(std::vector<ktl::kunique_ptr<T>>& vec, DeltaTime dt);
	static void tick(GameObject& go, DeltaTime dt);
	static void addTriggers(std::vector<Ptr<Trigger const>>& out, GameObject const& obj);

	struct AddToDrawList;
	struct Impl;
	ktl::kunique_ptr<Impl> m_impl{};

	struct {
		std::vector<ktl::kunique_ptr<GameObject>> objects{};
		State state{};
	} m_state{};
	ktl::kunique_ptr<Player> m_player{};
	Ptr<CookerPool> m_cookerPool{};
};
} // namespace rr
