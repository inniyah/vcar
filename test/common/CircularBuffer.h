#ifndef CIRCULAR_BUFFER_H_7D9F1CDE_D666_11E5_86B0_10FEED04CD1C
#define CIRCULAR_BUFFER_H_7D9F1CDE_D666_11E5_86B0_10FEED04CD1C

#include <cstdlib>
#include <cstdint>

namespace common {

template <typename T, size_t N>
class CircularBuffer {
public:
	explicit CircularBuffer() : m_head(0), m_tail(0), m_count(0) {
	}

	~CircularBuffer() {
	}

	inline bool pushFront(const T & item) {
		if (N != m_count) {
			// If not full: put data, then increase head
			m_data[m_head] = item;
			m_head = (m_head + 1) % N;
			++m_count;
			return true;
		}
		return false;
	}

	inline bool pushBack(const T & item) {
		if (N != m_count) {
			// If not full: decrease tail, then put data
			m_tail = (m_tail + N - 1) % N;
			m_data[m_tail] = item;
			++m_count;
			return true;
		}
		return false;
	}

	inline bool popFront(T & item) {
		if (0 != m_count) {
			// If not empty: decrease head, then get data
			m_head = (m_head + N - 1) % N;
			item = m_data[m_head];
			--m_count;
			return true;
		}
		return false;
	}

	inline bool popBack(T & item) {
		if (0 != m_count) {
			// If not empty: get data, then increase tail
			item = m_data[m_tail];
			m_tail = (m_tail + 1) % N;
			--m_count;
			return true;
		}
		return false;
	}

	inline bool peekFront(T & item) {
		if (0 != m_count) {
			// If not empty: get data from previous to head
			item = m_data[(m_head + N - 1) % N];
			return true;
		}
		return false;
	}

	inline bool peekBack(T & item) {
		if (0 != m_count) {
			// If not empty: get data from tail
			item = m_data[m_tail];
			return true;
		}
		return false;
	}

	inline static size_t maxItems() {
		return N;
	}

	inline size_t numItems() const {
		return m_count;
	}

	inline bool isEmpty() const {
		return (0 == m_count);
	}

	inline bool isFull() const {
		return (N == m_count);
	}

private:
	size_t  m_head;
	size_t  m_tail;
	size_t  m_count;
	T       m_data[N];
};

} // namespace common

#endif // CIRCULAR_BUFFER_H_7D9F1CDE_D666_11E5_86B0_10FEED04CD1C
