#pragma once
#include <vulkify/context/frame.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>

namespace rr {
struct MonoState {};

template <typename T = MonoState>
struct Instanced {
	using Mesh = vf::InstancedMesh<>;

	struct Entry {
		vf::DrawInstance instance{};
		T t{};
	};

	void add(glm::vec2 const position, T t = {}) {
		auto entry = Entry{.t = std::move(t)};
		entry.instance.transform.position = position;
		entries.push_back(std::move(entry));
	}

	mutable Mesh mesh{};
	std::vector<Entry> entries{};

	void draw(vf::Frame const& frame) const {
		refresh();
		frame.draw(mesh);
	}

	void refresh() const {
		mesh.storage.clear();
		mesh.storage.reserve(entries.size());
		for (auto const& entry : entries) { mesh.storage.push_back(entry.instance); }
	}
};
} // namespace rr
