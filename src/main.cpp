#include <engine/config.hpp>
#include <engine/context.hpp>
#include <engine/env.hpp>
#include <game/background.hpp>
#include <game/cooker_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/powerup.hpp>
#include <game/resources.hpp>
#include <util/collection.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <util/util.hpp>

namespace {
using namespace std::chrono_literals;

std::optional<rr::Context> make_context(int argc, char const* const argv[]) {
	auto env = rr::Env::make(argc, argv);
	auto config = rr::Config::load(env, "config.txt");
	auto builder = vf::Builder{};
	builder.setExtent(config.extent).setAntiAliasing(config.antiAliasing);
	// builder.setExtent({1920, 1080});
	auto vf = builder.build();
	if (!vf) {
		logger::error("Failed to create vulkify instance");
		return {};
	}
	auto ret = rr::Context{.env = std::move(env), .vf_context = std::move(*vf)};
	ret.capo_instance = capo::Instance::make();
	if (!ret.capo_instance) {
		logger::error("Failed to create capo instance");
		return {};
	}
	ret.config = std::move(config);
	ret.basis.scale = ret.basis_scale(ret.vf_context.framebufferExtent());
	ret.audio = *ret.capo_instance;
	return ret;
}

// template <typename T>
// [[maybe_unused]] void loadAllFrom(rr::World& world, rr::Collection<T>& out, std::string_view dir, std::string_view ext) {
// 	for (auto& file : rr::fileList(world.context().env, dir, ext)) {
// 		T t;
// 		if (world.load(t, file)) { out.push(std::move(t)); }
// 	}
// }

using rr::util::random_range;

template <int MaxIter = 100>
glm::vec2 random_cooker_pos(glm::vec2 const zone, glm::vec2 const offset, rr::CookerPool const& pool) {
	auto make_pos = [zone, offset] { return random_range(-zone, zone) + offset; };
	auto pos = make_pos();
	for (int loops{}; loops < MaxIter && pool.intersecting(rr::Trigger{pos, pool.triggerDiameter}); ++loops) { pos = make_pos(); }
	return pos;
}

void heal_player(rr::Game& game) {
	game.player().heal(1);
	game.audio().play(game.resources.sfx.power_up);
}

struct DebugControls : rr::KeyListener {
	rr::Ptr<rr::Game> game{};
	rr::Ptr<rr::Powerup> powerup{};

	void operator()(vf::KeyEvent const& key) override {
		auto const zone = 0.5f * game->layout.play_area.extent;
		auto const offset = +game->layout.play_area.offset;
		if (key(vf::Key::eEnter, vf::Action::ePress)) {
			auto const cooker_size = game->cooker_pool()->prefab().size;
			auto const cooker_zone = zone - 2.0f * cooker_size;
			game->cooker_pool()->spawn({random_cooker_pos(cooker_zone, offset, *game->cooker_pool()), vf::Time(random_range(2.0f, 5.0f))});
		}
		if (key(vf::Key::eBackslash, vf::Action::ePress) && powerup) {
			if (powerup->active()) {
				powerup->deactivate();
			} else {
				auto request = rr::Powerup::Request{
					.modify = &heal_player,
				};
				powerup->activate(std::move(request), powerup->random_position());
			}
		}
		if (key(vf::Key::eT, vf::Action::eRelease, vf::Mod::eCtrl)) { game->flags.flip(rr::Game::Flag::eRenderTriggers); }
		if (key(vf::Key::eW, vf::Action::eRelease, vf::Mod::eCtrl)) { game->context.vf_context.close(); }
		if (key(vf::Key::eP, vf::Action::eRelease, vf::Mod::eCtrl)) {
			auto const vsync = game->context.vf_context.vsync() == vf::VSync::eOff ? vf::VSync::eOn : vf::VSync::eOff;
			game->context.vf_context.setVSync(vsync);
		}
	}
};

rr::Resources load_resources(rr::Context& context) {
	auto loader = rr::Resources::Loader{context};
	auto ret = rr::Resources{};
	loader(ret.fonts.main, "fonts/main.ttf");
	loader(ret.textures.background, "textures/floor_tile.jpg");
	loader(ret.textures.cooker, "textures/cooker.png");
	loader(ret.textures.health, "textures/heart.png");
	loader(ret.animations.player, "animations/player.anim");
	loader(ret.animations.explode, "animations/explode.anim");
	loader(ret.sfx.tick_tock, "sfx/tick_tock.wav");
	loader(ret.sfx.explode, "sfx/explode.wav");
	loader(ret.sfx.collect, "sfx/beep.wav");
	loader(ret.sfx.power_up, "sfx/power_up.wav");
	return ret;
}

void run(rr::Context context) {
	auto resources = load_resources(context);
	auto game = rr::Game{context, resources};
	// game.audio().set_sfx_gain(0.2f);
	auto debug = DebugControls{};
	debug.game = &game;

	if constexpr (rr::debug_v) {
		game.attach(&debug);
		// game.flags.set(rr::Game::Flag::eRenderTriggers);
	}

	game.set(rr::Game::State::ePlay);
	debug.powerup = game.spawn<rr::Powerup>();
	auto const cooker_size = game.layout.basis.scale * glm::vec2{100.0f};
	game.cooker_pool()->set_prefab({cooker_size, resources.textures.cooker.handle()});

	context.vf_context.show();
	while (!context.vf_context.closing()) {
		auto frame = context.vf_context.frame();
		auto const& queue = frame.poll();
		game.handle(queue.events);
		game.tick(frame.dt());
		game.render(frame);
	}
}
} // namespace

int main(int argc, char* argv[]) {
	auto context = make_context(argc, argv);
	if (!context) { return EXIT_FAILURE; }
	run(std::move(*context));
}
