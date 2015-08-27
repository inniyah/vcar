#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <cassert>
#include <cstdlib>

namespace common {

template<typename T>
class Singleton {

public:

	Singleton(T & instance) {
		assert(NULL == spInstance);
		spInstance = &instance;
	}

	Singleton(T * p_instance) {
		assert(NULL == spInstance);
		assert(NULL != p_instance);
		spInstance = p_instance;
	}

	~Singleton() {
		spInstance = NULL;
	}

	static T & getInstance() {
		assert(NULL != spInstance);
		return *spInstance;
	}

private:

	static T * spInstance;

};

template<typename T> T * Singleton<T>::spInstance(NULL);

} // namespace common

#endif /* SINGLETON_H_ */
