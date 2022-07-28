#include <engine/config.hpp>
#include <engine/context.hpp>
#include <engine/env.hpp>
#include <game/background.hpp>
#include <game/consumable.hpp>
#include <game/cooker_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/resources.hpp>
#include <util/collection.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <util/util.hpp>

namespace {
std::optional<rr::Context> make_context(int argc, char const* const argv[]) {
	auto env = rr::Env::make(argc, argv);
	auto config = rr::Config::Scoped(rr::exe_path(env, "config.txt"));
	auto builder = vf::Builder{};
	builder.setExtent(config.config.extent).setAntiAliasing(config.config.antiAliasing);
	// builder.setExtent({1920, 1080});
	auto vf = builder.build();
	if (!vf) {
		logger::error("Failed to create vulkify instance");
		return {};
	}
	auto ret = rr::Context{.env = std::move(env), .vf_context = std::move(*vf)};
	ret.capo_instance = ktl::make_unique<capo::Instance>();
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

struct OneUp : rr::Consumable {
	void consume() override { game()->player().heal(1); }
};

using rr::util::random_range;

struct DebugControls : rr::KeyListener {
	rr::Ptr<rr::Game> game{};

	void operator()(vf::KeyEvent const& key) override {
		auto const zone = game->layout.play_area.extent * 0.5f;
		auto const offset = +game->layout.play_area.offset;
		if (key(vf::Key::eEnter, vf::Action::ePress)) { game->cooker_pool()->spawn({random_range(-zone, zone) + offset, vf::Time(random_range(2.0f, 5.0f))}); }
		if (key(vf::Key::eBackslash, vf::Action::ePress)) {
			auto consumable = game->spawn<OneUp>(random_range(-zone, zone) + offset);
			consumable->sprite.set_size({50.0f, 50.0f});
			consumable->trigger.diameter = 75.0f;
		}
		if (key(vf::Key::eW, vf::Action::eRelease, vf::Mod::eCtrl)) { game->context.vf_context.close(); }
		if (key(vf::Key::eP, vf::Action::eRelease, vf::Mod::eCtrl)) {
			auto const vsync = game->context.vf_context.vsync() == vf::VSync::eOff ? vf::VSync::eOn : vf::VSync::eOff;
			game->context.vf_context.setVSync(vsync);
		}
	}
};

void load_resources(rr::Game& out) {
	auto loader = rr::Resources::Loader{out.context};
	loader(out.resources().fonts.main, "fonts/main.ttf");
	loader(out.resources().textures.background, "textures/tilesf5.jpg");
}

void run(rr::Context context) {
	auto game = rr::Game{context};
	game.audio().set_sfx_gain(0.2f);
	load_resources(game);
	game.background()->set_texture(game.resources().textures.background);
	auto debug = DebugControls{};
	debug.game = &game;

	if constexpr (rr::debug_v) {
		game.attach(&debug);
		game.flags.set(rr::Game::Flag::eRenderTriggers);
	}

	auto sheet = rr::Sprite::Sheet{};
	auto tex = rr::util::make_texture(context, "textures/awesomeface.png");

	sheet.set_texture(std::move(tex)).set_uvs(2, 1);
	game.player().sprite.set_sheet(&sheet).set_uv_index(1);
	game.set(rr::Game::State::ePlay);

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
