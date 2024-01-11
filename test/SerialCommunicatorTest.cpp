#include "msg/test/BaseMsgTests.hpp"
#include "msg/test/DualPort.hpp"

void
testMsgSerialCommunication() {
	msg::DualPort<uint8_t> dualPort;
	auto ports = dualPort.GetPorts();
	std::unique_ptr<msg::Serial<uint8_t>> portA =
	    std::make_unique<msg::DualPort<uint8_t>::Port>(
		std::move(ports.first));
	std::unique_ptr<msg::Serial<uint8_t>> portB =
	    std::make_unique<msg::DualPort<uint8_t>::Port>(
		std::move(ports.second));
	const msg::Msg message =
	    msg::Msg(0x12EF, 5, std::vector<uint8_t>{ 1, 2, 3, 4, 5 });
	msg::checkMsgCommunication(message, portA, portB);
}

void
testMsgSerialCommunicator() {
	testMsgSerialCommunication();
}