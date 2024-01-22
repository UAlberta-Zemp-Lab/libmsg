#include "msg/test/BaseMsgTests.hpp"
#include "msg/test/DualPort.hpp"

void
testMsgSerialCommunication() {
	msg::DualPort<uint8_t> dualPort;
	auto ports = dualPort.GetPorts();
	const msg::Msg message =
	    msg::Msg(0x12EF, 5, std::vector<uint8_t>{ 1, 2, 3, 4, 5 });
	msg::checkMsgCommunication(message, ports.first, ports.second);
}

void
testMsgSerialCommunicator() {
	testMsgSerialCommunication();
}