#ifndef SAFE_CIRCULAR_BUFFER_H_5D03C388_D73A_11E5_A982_10FEED04CD1C
#define SAFE_CIRCULAR_BUFFER_H_5D03C388_D73A_11E5_A982_10FEED04CD1C

#include "CircularBuffer.h"
#include "SpinLock.h"

#include <cstdint>

namespace common {

template <typename T, size_t N>
class SafeCircularBuffer : protected CircularBuffer<T,N>, protected SpinLock {
public:
	explicit SafeCircularBuffer() {
	}

	~SafeCircularBuffer() {
	}

	inline bool pushFront(const T & item) {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::pushFront(item);
		SpinLock::unlock();
		return ret;
	}

	inline bool pushBack(const T & item) {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::pushBack(item);
		SpinLock::unlock();
		return ret;
	}

	inline bool popFront(T & item) {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::popFront(item);
		SpinLock::unlock();
		return ret;
	}

	inline bool popBack(T & item) {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::popBack(item);
		SpinLock::unlock();
		return ret;
	}

	inline bool peekFront(T & item) {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::peekFront(item);
		SpinLock::unlock();
		return ret;
	}

	inline bool peekBack(T & item) {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::peekBack(item);
		SpinLock::unlock();
		return ret;
	}

	inline static size_t maxItems() {
		return N;
	}

	inline size_t numItems() const {
		SpinLock::lock();
		size_t ret = CircularBuffer<T,N>::numItems();
		SpinLock::unlock();
		return ret;
	}

	inline bool isEmpty() const {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::isEmpty();
		SpinLock::unlock();
		return ret;
	}

	inline bool isFull() const {
		SpinLock::lock();
		bool ret = CircularBuffer<T,N>::isFull();
		SpinLock::unlock();
		return ret;
	}
};

} // namespace common

#endif // SAFE_CIRCULAR_BUFFER_H_5D03C388_D73A_11E5_A982_10FEED04CD1C
