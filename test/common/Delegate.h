#ifndef DELEGATE_H_8AE69622_4C11_11E5_95F0_10FEED04CD1C
#define DELEGATE_H_8AE69622_4C11_11E5_95F0_10FEED04CD1C

#include <cstdlib>

// See: http://blog.coldflake.com/posts/C++-delegates-on-steroids/

#if __cplusplus <= 199711L // not a C++11 compliant compiler
#error A C++11 compliant compiler is needed
#endif

namespace common {

// Usage:
//     A a;
//     auto d = Delegate<int, int>::from_function<A, &A::foo>(&a);
//     auto d2 = Delegate<int, int, int, char>::from_function<A, &A::bar>(&a);

template<typename return_type, typename... params>
class Delegate {
public:
	typedef return_type (*Type)(void * callee, params...);

	Delegate(void * callee, Type function) : fpCallee(callee) , fpCallbackFunction(function) { }
	Delegate(const Delegate & d) : fpCallee(d.fpCallee) , fpCallbackFunction(d.fpCallbackFunction) { }

	return_type operator()(params... xs) const {
		return (*fpCallbackFunction)(fpCallee, xs...);
	}

	Delegate & operator=(const Delegate & d) {
		fpCallee = d.fpCallee;
		fpCallbackFunction = d.fpCallbackFunction;
		return *this;
	}

	bool operator==(const Delegate & d) const {
		return ((fpCallee == d.fpCallee) && (fpCallbackFunction == d.fpCallbackFunction));
	}

	template <class T, return_type (T::*TMethod)(params...)>
	static Delegate fromObjectMethod(T * callee) {
		Delegate d(callee, &methodCaller<T, TMethod>);
		return d;
	}

protected:
	void * fpCallee;
	Type fpCallbackFunction;

	template <class T, return_type (T::*TMethod)(params...)>
	static return_type methodCaller(void* callee, params... xs) {
		T * p = static_cast<T*>(callee);
		return (p->*TMethod)(xs...);
	}
};

} // namespace common

#endif // DELEGATE_H_8AE69622_4C11_11E5_95F0_10FEED04CD1C
