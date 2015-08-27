#ifndef SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
#define SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C

#include "os.h"

#include "common/Singleton.h"
#include "common/LinkedList.h"

class System : public common::Singleton<System> {
public:
	System();
	~System();

	//ComSystem & getComSystem() {
	//	return m_ComSystem;
	//}

private:
	//ComSystem m_ComSystem;
};

#endif /* SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C */

