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
#include <util/util.hpp>

namespace {
std::optional<rr::Context> makeContext(int argc, char const* const argv[]) {
	auto env = rr::Env::make(argc, argv);
	auto config = rr::Config::Scoped(rr::exePath(env, "config.txt"));
	auto builder = vf::Builder{};
	builder.setExtent(config.config.extent).setAntiAliasing(config.config.antiAliasing);
	// builder.setExtent({1920, 1080});
	auto vf = builder.build();
	if (!vf) {
		logger::error("Failed to create vulkify instance");
		return {};
	}
	auto ret = rr::Context{.env = std::move(env), .vfContext = std::move(*vf)};
	ret.capoInstance = ktl::make_unique<capo::Instance>();
	ret.config = std::move(config);
	ret.basis.scale = ret.basisScale(ret.vfContext.framebufferExtent());
	ret.audio = *ret.capoInstance;
	return ret;
}

std::default_random_engine& randomEngine() {
	static auto eng = std::default_random_engine(std::random_device{}());
	return eng;
}

[[maybe_unused]] float randomRange(float min, float max) { return std::uniform_real_distribution<float>(min, max)(randomEngine()); }

template <std::integral T>
[[maybe_unused]] T randomRange(T min, T max) {
	return std::uniform_int_distribution<T>(min, max)(randomEngine());
}

[[maybe_unused]] glm::vec2 randomRange(glm::vec2 min, glm::vec2 max) { return {randomRange(min.x, max.x), randomRange(min.y, max.y)}; }

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

struct DebugControls : rr::KeyListener {
	rr::Ptr<rr::Game> game{};

	void operator()(vf::KeyEvent const& key) override {
		auto const zone = game->layout.playArea.extent * 0.5f;
		auto const offset = +game->layout.playArea.offset;
		if (key(vf::Key::eEnter, vf::Action::ePress)) {
			// auto cooker = game->spawn<rr::Cooker>(randomRange(-zone, zone));
			// cooker->text = vf::Text(game->context.vfContext, "cooker");
			// cooker->text.setFont(&game->resources().fonts.main);
			// cooker->text.setHeight(40);
			game->cookerPool()->spawn(randomRange(-zone, zone) + offset, vf::Time(randomRange(2.0f, 5.0f)));
		}
		if (key(vf::Key::eBackslash, vf::Action::ePress)) {
			auto consumable = game->spawn<OneUp>(randomRange(-zone, zone) + offset);
			consumable->sprite.setSize({50.0f, 50.0f});
			consumable->trigger.diameter = 75.0f;
		}
		if (key(vf::Key::eW, vf::Action::eRelease, vf::Mod::eCtrl)) { game->context.vfContext.close(); }
		if (key(vf::Key::eP, vf::Action::eRelease, vf::Mod::eCtrl)) {
			auto const vsync = game->context.vfContext.vsync() == vf::VSync::eOff ? vf::VSync::eOn : vf::VSync::eOff;
			game->context.vfContext.setVSync(vsync);
		}
	}
};

void loadResources(rr::Game& out) {
	auto loader = rr::Resources::Loader{out.context};
	loader(out.resources().fonts.main, "fonts/main.ttf");
	loader(out.resources().textures.background, "textures/tilesf5.jpg");
}

void run(rr::Context context) {
	// world.background()->open("textures/background.png");

	auto game = rr::Game{context};
	game.audio().setSfxGain(0.2f);
	loadResources(game);
	game.background()->setTexture(game.resources().textures.background);
	auto debug = DebugControls{};
	debug.game = &game;

	if constexpr (rr::debug_v) {
		game.attach(&debug);
		game.flags.set(rr::Game::Flag::eRenderTriggers);
	}

	auto sheet = rr::Sprite::Sheet{};
	auto tex = rr::util::makeTexture(context, "textures/awesomeface.png");

	sheet.setTexture(std::move(tex)).setUvs(2, 1);
	game.player().sprite.setSheet(&sheet).setUvIndex(1);
	game.set(rr::Game::State::ePlay);

	context.vfContext.show();
	while (!context.vfContext.closing()) {
		auto frame = context.vfContext.frame();
		auto const& queue = frame.poll();
		game.handle(queue.events);
		game.tick(frame.dt());
		game.render(frame);
	}
}
} // namespace

int main(int argc, char* argv[]) {
	auto context = makeContext(argc, argv);
	if (!context) { return EXIT_FAILURE; }
	run(std::move(*context));
}
