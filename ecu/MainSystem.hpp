#ifndef MAIN_SYSTEM_HPP_8AE698A2_4C11_11E5_95FF_10FEED04CD1C
#define MAIN_SYSTEM_HPP_8AE698A2_4C11_11E5_95FF_10FEED04CD1C

#include "os.h"
#include "BspSystem.h"
#include "MainSystem.h"
#include "rte.h"

template <void (ISwc::*TMethod)(Rte &)> void MainSystem::swcCall() {
	rte.readInputs(can01_rx);
	for (common::LinkedList<ISwc>::Iterator itr = m_SwcList.begin(); itr != m_SwcList.end(); ++itr) {
		((*itr).*TMethod)(rte);
	}
	rte.writeOutputs(can01_tx);
}

#endif // MAIN_SYSTEM_HPP_8AE698A2_4C11_11E5_95FF_10FEED04CD1C
