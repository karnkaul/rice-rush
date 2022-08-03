#pragma once
#include <cstddef>
#include <vector>

namespace rr::io {
class Instance {
  public:
	Instance(char const* arg0);
	~Instance();

	Instance& operator=(Instance&&) = delete;
};

bool mount_dir(char const* path);
bool mount_zip(char const* zip, char const* point = "");
bool exists(char const* uri);
bool load(std::vector<std::byte>& out, char const* uri);
} // namespace rr::io
