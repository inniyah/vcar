#ifndef DELEGATE_H_8AE69622_4C11_11E5_95F0_10FEED04CD1C
#define DELEGATE_H_8AE69622_4C11_11E5_95F0_10FEED04CD1C

#include <cstdlib>

// See: http://blog.coldflake.com/posts/C++-delegates-on-steroids/

namespace common {

#if __cplusplus <= 199711L // not a C++11 compliant compiler

// Usage:
//     A a;
//     typedef Delegate<int, int> IntDelegate;
//     IntDelegate d = IntDelegate::fromObjectMethod<A, &A::foo>(&a);

template<typename return_type, typename param_type>
class Delegate {

	typedef return_type (*Type)(void * callee, param_type);

public:

	Delegate() : fpCallee(NULL), fpCallbackFunction(Delegate::EmptyFunction) { }
	Delegate(void * callee, Type function) : fpCallee(callee), fpCallbackFunction(function) { }

	bool operator==(const Delegate & other) {
		return ((fpCallee != other.fpCallee) && (fpCallbackFunction != other.fpCallbackFunction));
	}

	template <class T, return_type (T::*TMethod)(param_type)>
	static Delegate fromObjectMethod(T * callee) {
		Delegate d(callee, &methodCaller<T, TMethod>);
		return d;
	}

	return_type operator()(param_type x) const {
		return (*fpCallbackFunction)(fpCallee, x);
	}

private:

	void * fpCallee;
	Type fpCallbackFunction;

	template <class T, return_type (T::*TMethod)(param_type)>
	static return_type methodCaller(void* callee, param_type x) {
		T * p = static_cast<T*>(callee);
		return (p->*TMethod)(x);
	}
	static return_type EmptyFunction(void * callee, param_type param) {
	}
};

#else // at least a C++11 compliant compiler

// Usage:
//     A a;
//     auto d = Delegate<int, int>::from_function<A, &A::foo>(&a);
//     auto d2 = Delegate<int, int, int, char>::from_function<A, &A::bar>(&a);
template<typename return_type, typename... params>
class Delegate {

		typedef return_type (*Type)(void* callee, params...);

public:

	Delegate(void * callee, Type function) : fpCallee(callee) , fpCallbackFunction(function) { }

	template <class T, return_type (T::*TMethod)(params...)>
	static Delegate from_function(T * callee) {
		Delegate d(callee, &methodCaller<T, TMethod>);
		return d;
	}

	return_type operator()(params... xs) const {
		return (*fpCallbackFunction)(fpCallee, xs...);
	}

private:

	void * fpCallee;
	Type fpCallbackFunction;

	template <class T, return_type (T::*TMethod)(params...)>
	static return_type methodCaller(void* callee, params... xs) {
		T * p = static_cast<T*>(callee);
		return (p->*TMethod)(xs...);
	}
};

#endif

} // namespace common

#endif // DELEGATE_H_8AE69622_4C11_11E5_95F0_10FEED04CD1C
