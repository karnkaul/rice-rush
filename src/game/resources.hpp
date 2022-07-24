#pragma once
#include <vulkify/ttf/ttf.hpp>

namespace rr {
struct Context;

struct Resources {
	struct {
		vf::Ttf main{};
	} fonts{};

	struct Loader;
};

struct Resources::Loader {
	Context& context;

	void operator()(vf::Ttf& out, std::string_view uri) const;
};
} // namespace rr
