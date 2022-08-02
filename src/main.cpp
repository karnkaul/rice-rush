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
	builder.set_title("Rice Rush").set_extent(config.extent).set_anti_aliasing(config.antiAliasing).set_vsyncs({config.vsync});
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
	ret.basis.scale = ret.basis_scale(ret.vf_context.framebuffer_extent());
	ret.audio = *ret.capo_instance;
	return ret;
}

using rr::util::random_range;

template <int MaxIter = 100>
glm::vec2 random_cooker_pos(glm::vec2 const zone, glm::vec2 const offset, rr::CookerPool const& pool) {
	auto make_pos = [zone, offset] { return random_range(-zone, zone) + offset; };
	auto pos = make_pos();
	for (int loops{}; loops < MaxIter && pool.intersecting(rr::Trigger{pos, pool.trigger_diameter}); ++loops) { pos = make_pos(); }
	return pos;
}

struct DebugControls : rr::KeyListener {
	rr::Ptr<rr::Game> game{};
	rr::Ptr<rr::Powerup> powerup{};

	void operator()(vf::KeyEvent const& key) override {
		if (key(vf::Key::eEnter, vf::Action::ePress)) { game->cooker_pool()->spawn(); }
		if (key(vf::Key::eBackslash, vf::Action::ePress) && powerup) {
			if (powerup->active()) {
				powerup->deactivate();
			} else {
				powerup->activate_heal(1);
				// powerup->activate_slowmo(0.5f);
				// powerup->activate_sweep();
			}
		}
		if (key(vf::Key::eT, vf::Action::eRelease, vf::Mod::eCtrl)) { game->flags.flip(rr::Game::Flag::eRenderTriggers); }
		if (key(vf::Key::eW, vf::Action::eRelease, vf::Mod::eCtrl)) { game->context.vf_context.close(); }
	}
};

bool load_resources(rr::Resources& out, rr::Context& context) {
	auto loader = rr::Resources::Loader{context};
	if (!loader(out, "manifest.txt")) {
		logger::warn("Failed to load game resources!");
		return false;
	}
	return true;
}

void run(rr::Context context) {
	auto resources = rr::Resources{};
	load_resources(resources, context);
	auto game = rr::Game{context, resources};
	auto debug = DebugControls{};
	debug.game = &game;

	if constexpr (rr::debug_v) {
		game.attach(&debug);
		// game.flags.set(rr::Game::Flag::eRenderTriggers);
	}

	game.set(rr::Game::State::ePlay);
	debug.powerup = game.powerup();

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
