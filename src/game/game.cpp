#include <engine/audio.hpp>
#include <engine/context.hpp>
#include <engine/trigger_renderer.hpp>
#include <game/background.hpp>
#include <game/cooker_pool.hpp>
#include <game/director.hpp>
#include <game/game.hpp>
#include <game/hud.hpp>
#include <game/player.hpp>
#include <game/powerup.hpp>
#include <game/resources.hpp>
#include <util/logger.hpp>
#include <algorithm>
#include <fstream>

namespace rr {
namespace {
std::uint64_t read_high_score(char const* path) {
	auto ret = std::uint64_t{};
	if (auto file = std::ifstream(path)) { file >> ret; }
	return ret;
}

void write_high_score(std::uint64_t score, char const* path) {
	if (auto file = std::ofstream(path)) { file << score << '\n'; }
}
} // namespace

struct Game::Impl {
	Resources resources{};
	rr::TriggerRenderer trigger_renderer;
	std::vector<Ptr<Trigger const>> triggers{};
	Keyboard keyboard{};
	Audio audio{};
	std::vector<Ptr<KeyListener>> listeners{};
	std::vector<Ptr<GameObject const>> draw_list{};
	std::uint64_t high_score{};

	Impl(Context& context) : trigger_renderer(context) {}
};

struct Game::AddToDrawList {
	Game const& game;
	void add(Ptr<GameObject const> go) const { game.m_impl->draw_list.push_back(go); }
	template <typename T>
	void add(std::vector<ktl::kunique_ptr<T>> const& vec) const {
		for (auto const& t : vec) { game.m_impl->draw_list.push_back(t.get()); }
	}

	template <typename... T>
	void operator()(T const&... t) const {
		(add(t), ...);
	}
};

Game::Game(Context& context, Resources& resources) : context(context), resources(resources), m_impl(ktl::make_unique<Impl>(context)) {
	layout.basis = context.basis;
	layout.play_area.extent = layout.basis.scale * layout.basis.space;
	layout.play_area.extent.y *= (1.0f - layout.n_pad_y);
	layout.play_area.offset.y = -0.5f * (layout.basis.scale * layout.basis.space.y - layout.play_area.extent.y);
	layout.hud.extent = {layout.play_area.extent.x, layout.basis.scale * layout.basis.space.y - layout.play_area.extent.y};
	layout.hud.offset.y = 0.5f * layout.play_area.extent.y;

	m_player = ktl::make_unique<Player>();
	setup(*m_player);
	m_player->sprite.instance().transform.position = layout.play_area.offset;
	m_player->name = context.config.playerName;

	m_background = ktl::make_unique<Background>(context.vf_context);
	m_background->set_texture(resources.textures.background, layout);

	m_impl->audio = *context.capo_instance;
	m_impl->audio.set_sfx_gain(context.config.sfx_gain);
	m_impl->high_score = read_high_score("high_score.rr");
}

void Game::attach(Ptr<KeyListener> listener) {
	if (!listener) { return; }
	listener->game = this;
	m_impl->listeners.push_back(listener);
}

void Game::detach(Ptr<KeyListener> listener) { std::erase(m_impl->listeners, listener); }

Keyboard const& Game::keyboard() const { return m_impl->keyboard; }
Audio& Game::audio() const { return m_impl->audio; }
float Game::sfx_gain() const { return context.config.sfx_gain; }

void Game::handle(std::span<vf::Event const> events) {
	for (auto const& event : events) {
		switch (event.type) {
		case vf::EventType::eKey: on_key(event.get<vf::EventType::eKey>()); break;
		default: break;
		}
	}
}

void Game::tick(vf::Time dt) {
	m_state.elapsed += dt;
	m_framerate.tick(dt);
	m_impl->audio.set_sfx_gain(context.config.sfx_gain);
	transfer_spawned();
	auto const dlt = DeltaTime{.real = dt, .scaled = dt * time_scale};
	tick(player(), dlt);
	tick(m_state.objects, dlt);

	auto const st = state();
	if (st == Game::State::eOver && player().start()) { set(Game::State::ePlay); }
	if (st == State::ePlay && player().health().hp <= 0) { set(State::eOver); }
}

void Game::render(vf::Frame const& frame) const {
	m_background->draw(frame);
	if (flags.test(Flag::eRenderTriggers)) {
		m_impl->triggers.clear();
		m_impl->triggers.push_back(&m_player->trigger);
		for (auto const& obj : m_state.objects) { add_triggers(m_impl->triggers, *obj); }
		m_impl->trigger_renderer.render(m_impl->triggers, frame);
	}
	m_impl->draw_list.clear();
	m_impl->draw_list.reserve(m_state.objects.size() + +1);
	AddToDrawList{*this}(m_state.objects, m_player.get());
	static constexpr auto cmp = [](Ptr<GameObject const> a, Ptr<GameObject const> b) { return a->layer < b->layer; };
	std::sort(m_impl->draw_list.begin(), m_impl->draw_list.end(), cmp);
	for (auto const* obj : m_impl->draw_list) { obj->draw(frame); }
}

void Game::set(State state) {
	if (state == m_state.state) { return; }
	switch (state) {
	case State::eOver: {
		m_state.state = state;
		logger::info("[Game] over; score: {}", player().score());
		m_impl->high_score = std::max(m_impl->high_score, player().score());
		write_high_score(m_impl->high_score, "high_score.rr");
		break;
	}
	case State::ePlay: {
		m_state = {};
		m_state.state = state;
		m_player->reset();
		m_cooker_pool = spawn<CookerPool>();
		m_hud = spawn<Hud>();
		m_powerup = spawn<Powerup>();
		spawn<Director>();
		logger::info("[Game] play");
		break;
	}
	default: break;
	}
}

std::uint64_t Game::high_score() const { return m_impl->high_score; }

void Game::on_key(vf::KeyEvent const& key) {
	for (auto* listener : m_impl->listeners) { (*listener)(key); }
	m_impl->keyboard.on_key(key);
}

void Game::setup(GameObject& out) {
	out.m_game = this;
	out.setup();
}

template <typename T>
void Game::tick(std::vector<ktl::kunique_ptr<T>>& vec, DeltaTime dt) {
	for (auto& t : vec) { tick(*t, dt); }
	std::erase_if(vec, [](auto const& t) { return t->m_destroyed; });
}

void Game::tick(GameObject& go, DeltaTime dt) { go.tick(dt); }

void Game::add_triggers(std::vector<Ptr<Trigger const>>& out, GameObject const& obj) { obj.add_triggers(out); }

void Game::transfer_spawned() {
	if (m_state.spawned.empty()) { return; }
	m_state.objects.reserve(m_state.objects.size() + m_state.spawned.size());
	std::move(m_state.spawned.begin(), m_state.spawned.end(), std::back_inserter(m_state.objects));
	m_state.spawned.clear();
}
} // namespace rr
