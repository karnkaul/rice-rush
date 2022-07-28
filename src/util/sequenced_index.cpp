#include <util/sequenced_index.hpp>
#include <algorithm>
#include <numeric>

namespace rr {
SequencedIndex& SequencedIndex::set(Sequence sequence, std::size_t index) {
	m_sequence = std::move(sequence);
	if (m_sequence.indices.empty()) {
		m_active = {};
		m_index = {};
		return *this;
	}
	m_active.index = std::min(index, m_sequence.indices.size() - 1);
	m_active.remain = m_sequence.duration / m_sequence.indices.size();
	m_index = m_sequence.indices[m_active.index];
	return *this;
}

void SequencedIndex::tick(vf::Time dt) {
	if (m_active.index >= m_sequence.indices.size()) { return; }
	m_active.remain -= dt;
	if (m_active.remain <= 0s) {
		m_active.index = (m_active.index + 1) % m_sequence.indices.size();
		m_index = m_sequence.indices[m_active.index];
		m_active.remain = m_sequence.duration / m_sequence.indices.size();
	}
}
} // namespace rr

std::vector<std::size_t> rr::make_sequence_indices(std::size_t count, std::size_t offset) {
	auto ret = std::vector<std::size_t>(count);
	std::iota(ret.begin(), ret.end(), offset);
	return ret;
}
