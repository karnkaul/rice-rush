#pragma once
#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <span>

namespace rr {
class ByteArray {
  public:
	ByteArray() = default;

	explicit ByteArray(std::size_t length) : m_data(std::make_unique_for_overwrite<std::byte[]>(length)), m_capacity(length), m_size(length) {}

	ByteArray& resize_for_overwrite(std::size_t length) {
		if (m_capacity < length) { *this = ByteArray(length); }
		return *this;
	}

	ByteArray& overwrite(void const* data, std::size_t length) {
		resize_for_overwrite(length);
		assert(m_data.get() && data && length > 0);
		std::memcpy(m_data.get(), data, m_size = length);
		return *this;
	}

	std::byte const* data() const noexcept { return m_data.get(); }
	std::byte* data() noexcept { return m_data.get(); }
	std::size_t size() const noexcept { return m_size; }

	std::span<std::byte const> span() const { return {data(), size()}; }
	operator std::span<std::byte const>() const { return span(); }

	void swap(ByteArray& rhs) { std::swap(m_data, rhs.m_data); }

  private:
	std::unique_ptr<std::byte[]> m_data{};
	std::size_t m_capacity{};
	std::size_t m_size{};
};
} // namespace rr
