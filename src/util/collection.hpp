#pragma once
#include <cassert>
#include <random>
#include <vector>

namespace rr {
template <typename Type>
class Collection {
  public:
	using type = Type;

	void push(Type t) { m_vec.push_back(std::move(t)); }
	std::size_t size() const { return m_vec.size(); }
	bool empty() const { return m_vec.empty(); }
	void clear() { m_vec.clear(); }

	Type& get() { return const_cast<Type&>(static_cast<Collection const&>(*this).get()); }

	Type const& get() const {
		assert(!m_vec.empty());
		return m_vec[m_index];
	}

	Type& next() {
		assert(!m_vec.empty());
		m_index = nextIndex();
		return get();
	}

	Type& random() {
		assert(!m_vec.empty());
		m_index = randomIndex();
		return get();
	}

  private:
	std::size_t nextIndex() const { return (m_index + 1) % m_vec.size(); }

	std::size_t randomIndex() const {
		static auto eng = std::default_random_engine(std::random_device{}());
		return m_vec.size() == 1 ? 0 : std::uniform_int_distribution<std::size_t>(0, m_vec.size() - 1)(eng);
	}

	std::vector<Type> m_vec{};
	std::size_t m_index{};
};
} // namespace rr
