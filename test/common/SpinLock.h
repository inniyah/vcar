#ifndef SPINLOCK_H_0DF2E35A_D73A_11E5_9F8C_10FEED04CD1C
#define SPINLOCK_H_0DF2E35A_D73A_11E5_9F8C_10FEED04CD1C

#include <atomic>
#include <xmmintrin.h>

namespace common {

class SpinLock {
public:
		SpinLock() : m_lock(false) {}

		void lock() {
		// heavy test here for the usual uncontested case
		bool exp = false;
		if (!m_lock.compare_exchange_weak(exp, true, std::memory_order_acquire)) {
			for (;;) {
				// processor spin loop hint
				_mm_pause();
				// spin on mov instead of lock instruction
				if (!m_lock.load(std::memory_order_relaxed)) {
					// heavy test now that we suspect success
					exp = false;
					if (m_lock.compare_exchange_weak(exp, true, std::memory_order_acquire)) {
						return;
					}
				}
			}
		}
	}

	void unlock() {
		m_lock.store(false, std::memory_order_release);
	}

private:
	std::atomic_bool m_lock;
};

} // namespace common

#endif // SPINLOCK_H_0DF2E35A_D73A_11E5_9F8C_10FEED04CD1C
