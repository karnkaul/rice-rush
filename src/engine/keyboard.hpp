#pragma once
#include <ktl/hash_table.hpp>
#include <vulkify/instance/key_event.hpp>
#include <unordered_map>

namespace rr {
class Keyboard {
  public:
	void on_key(vf::KeyEvent const& key) { m_pressed[key.key] = key.action != vf::Action::eRelease; }

	bool pressed(vf::Key const key) const {
		if (auto it = m_pressed.find(key); it != m_pressed.end()) { return it->second; }
		return false;
	}

	void clear() { m_pressed.clear(); }

  private:
	std::unordered_map<vf::Key, bool> m_pressed{};
};
} // namespace rr
