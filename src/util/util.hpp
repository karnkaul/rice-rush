#pragma once
#include <vulkify/core/time.hpp>

namespace vf {
class Texture;
}

namespace rr {
class Context;

namespace util {
std::string formatElapsed(vf::Time elapsed);

vf::Texture makeTexture(Context const& context, std::string_view uri);
} // namespace util
} // namespace rr
