#include "rte.h"
#include "can01.dbc.h"

void Rte::init() {
}

void Rte::readInputs(CanBus_can01::RxMsgs & can01_rx) {
		In.Gear          = can01_rx.Msg_driving_controls.getSignal_gear();
		In.Acceleration  = can01_rx.Msg_driving_controls.getSignal_acceleration();
		In.Brake         = can01_rx.Msg_driving_controls.getSignal_brake();
		In.SteeringWheel = can01_rx.Msg_driving_controls.getSignal_wheel();
}

void Rte::writeOutputs(CanBus_can01::TxMsgs & can01_tx) {
}
