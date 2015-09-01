#ifndef LINKED_DELEGATE_LIST_H_8AE69776_4C11_11E5_95F8_10FEED04CD1C
#define LINKED_DELEGATE_LIST_H_8AE69776_4C11_11E5_95F8_10FEED04CD1C

#include "LinkedList.h"
#include "Delegate.h"

namespace common {

#if __cplusplus <= 199711L // not a C++11 compliant compiler

template<typename param_type>
class DelegateListNode :
	public LinkedListNode<DelegateListNode<param_type> >,
	public Delegate<void, param_type>
{
};

template<typename param_type>
class DelegateListIterator :
	public LinkedListIterator<DelegateListNode<param_type> >
{
};

template<typename param_type>
class DelegateList :
	public LinkedList<DelegateListNode<param_type> >
{
public:
	typedef DelegateListNode<param_type>                            Node;
	typedef const DelegateListNode<param_type>                      ConstNode;
	typedef LinkedListIterator<DelegateListNode<param_type> >       Iterator;
	typedef LinkedListIterator<const DelegateListNode<param_type> > ConstIterator;

	inline void insert(Node & d) {
		this->push_back(d);
	}

	inline void remove(Node & d) {
		this->remove(d);
	}

	void operator()(param_type x) const {
		for (
			ConstIterator itr = LinkedList<DelegateListNode<param_type> >::begin();
			itr != LinkedList<DelegateListNode<param_type> >::end();
			++itr
		) {
			itr->operator()(x);
		}
	}
};

#else // at least a C++11 compliant compiler

template<typename... params>
class DelegateListNode :
	public LinkedListNode<DelegateListNode>,
	public Delegate<void, params... xs>
{
};

template<typename... params>
class DelegateListIterator :
	public LinkedListIterator<DelegateListNode<params... xs> >
{
};

template<typename... params>
class DelegateList :
	public LinkedList<DelegateListNode<params... xs> >
{
public:
	typedef DelegateListNode<param_type>     Node;
	typedef DelegateListIterator<param_type> Iterator;

	inline void insert(Node & d) {
		push_back(d);
	}

	inline void remove(Node & d) {
		remove(d);
	}

	void operator()(params... xs) const {
		for (
			Iterator itr = LinkedList<DelegateListNode<param_type> >::begin();
			itr != LinkedList<DelegateListNode<param_type> >::end();
			++itr
		) {
			itr->operator()(xs...);
		}
	}
};

#endif

} // namespace common

#endif // LINKED_DELEGATE_LIST_H_8AE69776_4C11_11E5_95F8_10FEED04CD1C
