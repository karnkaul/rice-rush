#pragma once
#include <vulkify/core/time.hpp>

namespace vf {
class Texture;
}

namespace rr {
class Context;

namespace util {
std::string format_elapsed(vf::Time elapsed);

vf::Texture make_texture(Context const& context, std::string_view uri);
} // namespace util
} // namespace rr
