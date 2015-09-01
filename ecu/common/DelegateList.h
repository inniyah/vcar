#ifndef LINKED_DELEGATE_LIST_H_8AE69776_4C11_11E5_95F8_10FEED04CD1C
#define LINKED_DELEGATE_LIST_H_8AE69776_4C11_11E5_95F8_10FEED04CD1C

#include "LinkedList.h"
#include "Delegate.h"

namespace common {

#if __cplusplus <= 199711L // not a C++11 compliant compiler

template<typename return_type, typename param_type>
class DelegateListNode :
	public LinkedListNode<DelegateListNode<return_type, param_type> >,
	public Delegate<return_type, param_type>
{
};

template<typename return_type, typename param_type>
class DelegateList :
	public LinkedList<DelegateListNode<return_type, param_type> >
{
};

#else // at least a C++11 compliant compiler

template<typename return_type, typename... params>
class DelegateListNode :
	public LinkedListNode<DelegateListNode>,
	public Delegate<return_type, params... xs>
{
};

template<typename return_type, typename... params>
class DelegateListNode :
	public LinkedListNode<DelegateListNode<return_type, params... xs> >
{
};

#endif

} // namespace common

#endif // LINKED_DELEGATE_LIST_H_8AE69776_4C11_11E5_95F8_10FEED04CD1C
