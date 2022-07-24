#pragma once
#include <vulkify/graphics/primitives/instanced_mesh.hpp>

namespace rr {
template <typename T>
struct Instanced {
	using Mesh = vf::InstancedMesh<>;

	struct Entry {
		vf::DrawInstance instance{};
		T t{};
	};

	void add(glm::vec2 const position, T t) {
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
		mesh.instances.clear();
		mesh.instances.reserve(entries.size());
		for (auto const& entry : entries) { mesh.instances.push_back(entry.instance); }
	}
};
} // namespace rr
