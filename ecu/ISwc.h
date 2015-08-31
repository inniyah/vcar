#ifndef ISWC_H_
#define ISWC_H_

#include "common/LinkedList.h"

struct Rte;

class ISwc : public common::LinkedListNode<ISwc> {
public:
	virtual void init() = 0;
	virtual void update(Rte & rte) = 0;
	virtual void shutdown() = 0;
};

#endif // ISWC_H_
