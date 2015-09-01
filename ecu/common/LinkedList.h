#ifndef LINKED_LIST_H_8AE6964A_4C11_11E5_95F1_10FEED04CD1C
#define LINKED_LIST_H_8AE6964A_4C11_11E5_95F1_10FEED04CD1C

#include <cassert>
#include <cstdlib>

#include "StaticChecks.h"

namespace common {

template<typename T> class LinkedListNode;
template<typename T> class LinkedListIterator;
template<typename T> class LinkedList;

template<typename T> bool operator==(const LinkedListIterator<T> &, const LinkedListIterator<T> &);
template<typename T> bool operator!=(const LinkedListIterator<T> &, const LinkedListIterator<T> &);

// Linked List Node

template<typename T>
class LinkedListNode {

	friend class LinkedList<T>;

public:
	LinkedListNode() : m_Next(NULL), m_List(NULL) {
	}

	T * getNext() const {
		return m_Next;
	}

	void setNext(T * next) {
		m_Next = next;
	}

	bool isPartOfList() {
		return (NULL != m_List);
	}

protected:
	void addToList(LinkedList<T> & list) {
		assert(NULL == m_List);
		m_List = &list;
	}

	void removeFromList(LinkedList<T> & list) {
		assert(m_List == &list);
		m_List = NULL;
	}

	LinkedListNode(const LinkedListNode &);
	LinkedListNode & operator=(const LinkedListNode &);

	T * m_Next;
	LinkedList<T> * m_List;
};

// Linked List Iterator

template<typename T>
class LinkedListIterator {
public:
	LinkedListIterator(T * curr) : m_Current(curr) {
	}

	LinkedListIterator(const LinkedListIterator & i) : m_Current(i.m_Current) {
	}

	LinkedListIterator & operator=(const LinkedListIterator & i) {
		m_Current = i.m_Current;
		return *this;
	}

	LinkedListIterator & operator++() {
		m_Current = m_Current->getNext();
		return *this;
	}

	T & operator*() {
		return * m_Current;
	}

	T * operator->() {
		return m_Current;
	}

private:
	friend bool operator==<T>(const LinkedListIterator<T> &, const LinkedListIterator<T> &);
	friend bool operator!=<T>(const LinkedListIterator<T> &, const LinkedListIterator<T> &);

	T * m_Current;
};

template<typename T>
inline bool operator==(const LinkedListIterator<T> & x, const LinkedListIterator<T> & y) {
	return (x.m_Current == y.m_Current);
}

template<typename T>
inline bool operator!=(const LinkedListIterator<T> & x, const LinkedListIterator<T> & y) {
	return !(x == y);
}

// Linked List

template<typename T>
class LinkedList : common::DerivedFrom<T, LinkedListNode<T> > {
public:
	LinkedList() : m_First(NULL), m_Last(NULL) {
		clear();
	}

	typedef T                           Type;
	typedef LinkedListIterator<T>       Iterator;
	typedef LinkedListIterator<const T> ConstIterator;
	typedef LinkedListNode<T>           Node;
	typedef LinkedListNode<const T>     ConstNode;

	unsigned int size() const {
		unsigned int size = (NULL != m_First) ? 1 : 0;
		for (const T * pNode = m_First; pNode != m_Last; pNode = pNode->getNext()) {
			++size;
		}
		return size;
	}

	bool empty() const {
		return (NULL == m_First);
	}

	void clear() {
		while (!empty()) {
			pop_front();
		}
		m_First = NULL;
		m_Last = NULL;
	}

	const T & front() const {
		assert(m_First);
		return * m_First;
	}

	T &  front() {
		return const_cast<T &>(static_cast<const T &>(*this).front());
	}

	const T &  back() const {
		assert(m_Last);
		return *m_Last;
	}

	T &  back() {
		return const_cast<T &>(static_cast<const T &>(*this).back());
	}

	void push_front(T & node) {
		if (contains_node(&node)) {
			return;
		}
		node.addToList(*this);
		node.setNext(m_First);
		m_First = &node;
		if (!m_Last) {
			m_Last = m_First;
			m_First->setNext(NULL);
		}
	}

	void push_back(T & node) {
		if (empty()) {
			m_First = &node;
		} else {
			if (contains_node(&node)) {
				return;
			}
			m_Last->setNext(&node);
		}
		node.addToList(*this);
		m_Last = &node;
		m_Last->setNext(NULL);
	}

	void pop_front() {
		if (!m_First) {
			return;
		}
		m_First->removeFromList(*this);
		m_First = m_First->getNext();
		if (!m_First) {
			m_Last = NULL;
		}
	}

	void pop_back();

	LinkedListIterator<T> begin() {
		return iterator(m_First);
	}

	LinkedListIterator<const T> begin() const {
		return ConstIterator(m_First);
	}

	LinkedListIterator<T> end() {
		return LinkedListIterator<T>(NULL);
	}

	LinkedListIterator<const T> end() const {
		return LinkedListIterator<const T>(NULL);
	}

	LinkedListIterator<T> erase(LinkedListIterator<T> pos) {
		if (!m_First) {
			return end();
		}
		LinkedListIterator<T> next = pos;
		++next;
		if (pos == begin()) {
			pop_front();
		} else {
			pos->removeFromList(*this);
			T * pNode = findPredecessor(pos.operator->());
			assert(pNode);
			if (pNode->getNext() == m_Last) {
				m_Last = pNode;
			}
			pNode->setNext(pos->getNext());
		}
		return next;
	}

	LinkedListIterator<T> insert(LinkedListIterator<T> pos, T & value) {
		if (contains_node(&value)) {
			return pos;
		}
		if (empty() || (begin() == pos)) {
			push_front(value);
		} else if (end() == pos) {
			push_back(value);
		} else {
			value.addToList(*this);
			T* n = findPredecessor(pos.operator->());
			assert(n);
			value.setNext(n->getNext());
			n->setNext(&value);
		}
		return iterator(&value);
	}

	void remove(const T & value) {
		if (&value == m_First) {
			pop_front();
		} else if (&value == m_Last) {
			pop_back();
		} else {
			const_cast<T&>(value).removeFromList(*this);
			T * n = findPredecessor(&value);
			if (n) {
				n->setNext(value.getNext());
			}
		}
	}

	bool contains(const T & value) const {
		return contains_node(&value);
	}

private:
	LinkedList(const LinkedList &);
	LinkedList & operator=(const LinkedList &);

	T * findPredecessor(const T * node) const {
		T * result = m_First;
		while ((NULL != result) && (result->getNext() != node)) {
			result = result->getNext();
		}
		return result;
	}

	bool contains_node(const T * node) const;

	T * m_First;
	T * m_Last;
};

template<typename T>
bool LinkedList<T>::contains_node(const T * node) const {
	T * n = m_First;
	while (NULL != n) {
		if (node == n) {
			return true;
		}
		n = n->getNext();
	}
	return false;
}

template<typename T>
void LinkedList<T>::pop_back() {
	if (NULL == m_Last) {
		return;
	}
	m_Last->removeFromList(*this);
	if (m_First == m_Last) {
		clear();
	} else {
		m_Last = findPredecessor(m_Last);
		assert(NULL != m_Last);
		m_Last->setNext(NULL);
	}
}

} // namespace common

#endif // LINKED_LIST_H_8AE6964A_4C11_11E5_95F1_10FEED04CD1C
