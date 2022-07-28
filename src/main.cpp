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
#include <util/sequenced_index.hpp>
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

rr::Resources load_resources(rr::Context& context) {
	auto loader = rr::Resources::Loader{context};
	auto ret = rr::Resources{};
	loader(ret.fonts.main, "fonts/main.ttf");
	loader(ret.textures.background, "textures/tilesf5.jpg");
	return ret;
}

void run(rr::Context context) {
	auto resources = load_resources(context);
	auto game = rr::Game{context, resources};
	game.audio().set_sfx_gain(0.2f);
	auto debug = DebugControls{};
	debug.game = &game;

	if constexpr (rr::debug_v) {
		game.attach(&debug);
		game.flags.set(rr::Game::Flag::eRenderTriggers);
	}

	auto player_sheet = rr::Sprite::Sheet{rr::util::make_texture(context, "textures/player/1.png")};
	player_sheet.set_uvs(1, 6);

	auto cooker_sheet = rr::Sprite::Sheet{rr::util::make_texture(context, "textures/cooker.png")};

	game.set(rr::Game::State::ePlay);
	auto const player_size = game.layout.basis.scale * glm::vec2{150.0f};
	game.player().sprite.set_sheet(&player_sheet).set_size(player_size);
	auto seq = rr::SequencedIndex::Sequence{};
	seq.indices = rr::make_sequence_indices(player_sheet.uv_count());
	seq.duration = std::chrono::milliseconds(300);
	game.player().run_anim.set(std::move(seq));
	auto const cooker_size = game.layout.basis.scale * glm::vec2{75.0f};
	game.cooker_pool()->sprite.set_sheet(&cooker_sheet).set_size(cooker_size);

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
