#include <build_version.hpp>
#include <engine/context.hpp>
#include <game/background.hpp>
#include <game/cooker_pool.hpp>
#include <game/game.hpp>
#include <game/player.hpp>
#include <game/powerup.hpp>
#include <game/resources.hpp>
#include <util/io.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <util/util.hpp>
#include <filesystem>

namespace rr {
namespace {
namespace fs = std::filesystem;

fs::path find_data(fs::path start) {
	while (!start.empty() && start.parent_path() != start) {
		auto ret = start / "data";
		if (fs::is_directory(ret)) { return ret; }
		start = start.parent_path();
	}
	return {};
}

bool parse_arg(std::string_view const arg) {
	if (arg == "--version") {
		auto const& v = version_v;
		logger::print_verbatim(logger::Pipe::StdOut, ktl::kformat("Rice Rush v{}.{}.{}", v.major, v.minor, v.patch).c_str());
		return false;
	}
	return true;
}

std::optional<int> parse_args(int argc, char const* const argv[]) {
	if (argc < 1) {
		logger::error("Fatal error: no arguments passed to main");
		return EXIT_FAILURE;
	}
	for (int i = 1; i < argc; ++i) {
		if (!parse_arg(argv[i])) { return EXIT_SUCCESS; }
	}
	return {};
}

std::optional<Context> make_context(std::string_view const arg0) {

	auto const exe = fs::path{arg0};
	auto const exe_dir = fs::absolute(exe.parent_path());
	auto config = Config::load(exe_dir.generic_string(), "config.txt");
	auto builder = vf::Builder{};
	builder.set_title("Rice Rush").set_extent(config.extent).set_anti_aliasing(config.antiAliasing).set_vsyncs({config.vsync});
	auto vf = builder.build();
	if (!vf) {
		logger::error("Failed to create vulkify instance");
		return {};
	}
	auto ret = Context{.vf_context = std::move(*vf)};
	ret.capo_instance = capo::Instance::make();
	if (!ret.capo_instance) {
		logger::error("Failed to create capo instance");
		return {};
	}
	ret.config = std::move(config);
	ret.basis.scale = ret.basis_scale(ret.vf_context.framebuffer_extent());
	ret.audio = *ret.capo_instance;

	if (auto data = find_data(exe_dir); !data.empty()) { io::mount_dir(data.generic_string().c_str()); }

	return ret;
}

struct DebugControls : KeyListener {
	Ptr<Game> game{};
	Ptr<Powerup> powerup{};

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
		if (key(vf::Key::eT, vf::Action::eRelease, vf::Mod::eCtrl)) { game->flags.flip(Game::Flag::eRenderTriggers); }
		if (key(vf::Key::eW, vf::Action::eRelease, vf::Mod::eCtrl)) { game->context.vf_context.close(); }
	}
};

bool load_resources(Resources& out, Context& context) {
	auto loader = Resources::Loader{context};
	if (!loader(out, "manifest.txt")) {
		logger::warn("Failed to load game resources!");
		return false;
	}
	return true;
}

void run(Context context) {
	auto resources = Resources{};
	load_resources(resources, context);
	auto game = Game{context, resources};
	auto debug = DebugControls{};
	debug.game = &game;

	if constexpr (logger::debug_v) {
		game.attach(&debug);
		// game.flags.set(Game::Flag::eRenderTriggers);
	}

	game.set(Game::State::ePlay);
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
} // namespace rr

int main(int argc, char* argv[]) {
	auto io_inst = rr::io::Instance(argv[0]);
	if (auto ret = rr::parse_args(argc, argv)) { return *ret; }
	auto context = rr::make_context(argv[0]);
	if (!context) { return EXIT_FAILURE; }
	rr::run(std::move(*context));
}
