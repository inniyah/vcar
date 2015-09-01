#ifndef SWC_H_8AE6987A_4C11_11E5_95FE_10FEED04CD1C
#define SWC_H_8AE6987A_4C11_11E5_95FE_10FEED04CD1C

#include "ISwc.h"

class SwcBackLight: public ISwc {
public:
	SwcBackLight();
	virtual void init(Rte & rte);
	virtual void cyclic(Rte & rte);
	virtual void shutdown(Rte & rte);
};

#endif // SWC_H_8AE6987A_4C11_11E5_95FE_10FEED04CD1C
