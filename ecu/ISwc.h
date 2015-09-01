#ifndef ISWC_H_8AE69848_4C11_11E5_95FD_10FEED04CD1C
#define ISWC_H_8AE69848_4C11_11E5_95FD_10FEED04CD1C

#include "common/LinkedList.h"

struct Rte;

class ISwc : public common::LinkedListNode<ISwc> {
public:
	virtual void init(Rte & rte) = 0;
	virtual void cyclic(Rte & rte) = 0;
	virtual void shutdown(Rte & rte) = 0;
};

#endif // ISWC_H_8AE69848_4C11_11E5_95FD_10FEED04CD1C
