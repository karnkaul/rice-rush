#include <engine/audio.hpp>
#include <engine/context.hpp>
#include <engine/trigger_renderer.hpp>
#include <game/cooker_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <util/logger.hpp>
#include <algorithm>

namespace rr {
struct Game::Impl {
	Resources resources{};
	rr::TriggerRenderer triggerRenderer;
	std::vector<Ptr<Trigger const>> triggers{};
	Keyboard keyboard{};
	Audio audio{};
	std::vector<Ptr<KeyListener>> listeners{};
	std::vector<Ptr<GameObject const>> drawList{};

	Impl(Context& context) : triggerRenderer(context) {}
};

struct Game::AddToDrawList {
	Game const& game;
	void add(Ptr<GameObject const> go) const { game.m_impl->drawList.push_back(go); }
	template <typename T>
	void add(std::vector<ktl::kunique_ptr<T>> const& vec) const {
		for (auto const& t : vec) { game.m_impl->drawList.push_back(t.get()); }
	}

	template <typename... T>
	void operator()(T const&... t) const {
		(add(t), ...);
	}
};

Game::Game(Context& context) : context(context), m_impl(ktl::make_unique<Impl>(context)) {
	layout.basis = context.basis;
	layout.playArea = layout.basis.scale * layout.basis.space;
	layout.playArea.y *= (1.0f - layout.nPadY);

	m_player = ktl::make_unique<Player>();
	setup(*m_player, {});
}

void Game::attach(Ptr<KeyListener> listener) {
	if (!listener) { return; }
	listener->game = this;
	m_impl->listeners.push_back(listener);
}

void Game::detach(Ptr<KeyListener> listener) { std::erase(m_impl->listeners, listener); }

Keyboard const& Game::keyboard() const { return m_impl->keyboard; }
Resources& Game::resources() const { return m_impl->resources; }
Audio& Game::audio() const { return m_impl->audio; }

void Game::handle(std::span<vf::Event const> events) {
	for (auto const& event : events) {
		switch (event.type) {
		case vf::EventType::eKey: onKey(event.get<vf::EventType::eKey>()); break;
		default: break;
		}
	}
}

void Game::tick(vf::Time dt) {
	auto const dlt = DeltaTime{.real = dt, .scaled = dt * timeScale};
	if (m_state.state == State::ePlay) {
		tick(player(), dlt);
		if (player().health().hp <= 0) { set(State::eOver); }
	}
	tick(m_state.objects, dlt);
}

void Game::render(vf::Frame const& frame) const {
	if (flags.test(Flag::eRenderTriggers)) {
		m_impl->triggers.clear();
		m_impl->triggers.push_back(&m_player->trigger);
		for (auto const& obj : m_state.objects) { addTriggers(m_impl->triggers, *obj); }
		m_impl->triggerRenderer.render(m_impl->triggers, frame);
	}
	m_impl->drawList.clear();
	m_impl->drawList.reserve(m_state.objects.size() + +1);
	AddToDrawList{*this}(m_state.objects, m_player.get());
	static constexpr auto cmp = [](Ptr<GameObject const> a, Ptr<GameObject const> b) { return a->layer < b->layer; };
	std::sort(m_impl->drawList.begin(), m_impl->drawList.end(), cmp);
	for (auto const* obj : m_impl->drawList) { obj->draw(frame); }
}

void Game::set(State state) {
	if (state == m_state.state) { return; }
	switch (state) {
	case State::eOver: {
		m_state.state = state;
		logger::info("[Game] over; score: {}", player().score());
		break;
	}
	case State::ePlay: {
		m_state = {};
		m_state.state = state;
		m_player->reset();
		m_cookerPool = spawn<CookerPool>({});
		logger::info("[Game] play");
		break;
	}
	default: break;
	}
}

void Game::onKey(vf::KeyEvent const& key) {
	for (auto* listener : m_impl->listeners) { (*listener)(key); }
	m_impl->keyboard.onKey(key);
}

void Game::setup(GameObject& out, glm::vec2 position) {
	out.sprite = Sprite(context);
	out.sprite.instance().transform.position = position;
	out.m_game = this;
	out.setup();
}

template <typename T>
void Game::tick(std::vector<ktl::kunique_ptr<T>>& vec, DeltaTime dt) {
	for (auto& t : vec) { tick(*t, dt); }
	std::erase_if(vec, [](auto const& t) { return t->m_destroyed; });
}

void Game::tick(GameObject& go, DeltaTime dt) { go.tick(dt); }

void Game::addTriggers(std::vector<Ptr<Trigger const>>& out, GameObject const& obj) { obj.addTriggers(out); }
} // namespace rr
