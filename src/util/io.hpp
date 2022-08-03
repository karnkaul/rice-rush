#pragma once

namespace ktl {
class byte_array;
}

namespace rr {
namespace io {
class Instance {
  public:
	Instance(char const* arg0);
	~Instance();

	Instance& operator=(Instance&&) = delete;
};

bool mount_dir(char const* path);
bool mount_zip(char const* zip, char const* point = "");
bool exists(char const* uri);
bool load(ktl::byte_array& out, char const* uri);
} // namespace io
} // namespace rr
