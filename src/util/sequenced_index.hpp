#pragma once
#include <util/ptr.hpp>
#include <vulkify/core/time.hpp>
#include <vector>

namespace rr {
using namespace std::chrono_literals;

class SequencedIndex {
  public:
	struct Sequence {
		std::vector<std::size_t> indices{};
		vf::Time duration{1s};
	};

	SequencedIndex& set(Sequence sequence, std::size_t index = 0);

	void tick(vf::Time dt);
	std::size_t index() const { return m_index; }

  protected:
	struct Active {
		std::size_t index{};
		vf::Time remain{};
	};

	Sequence m_sequence{};
	Active m_active{};
	std::size_t m_index{};
};

std::vector<std::size_t> make_sequence_indices(std::size_t count, std::size_t offset = 0);
} // namespace rr
