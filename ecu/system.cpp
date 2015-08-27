#include "system.h"

#include "can01.dbc.h"

CanBus_can01::SndMsgs can01_snd;
CanBus_can01::RcvMsgs can01_rcv;

System::System() : common::Singleton<System>(this) {
}

System::~System() {
}
