#include "msg/test/BaseMsgTests.hpp"
#include "msg/test/DualPort.hpp"

void
testMsgSerialCommunication() {
	auto ports = msg::Port<uint8_t>::GetPair();
	const msg::Msg message =
	    msg::Msg(0x12EF, 5, std::vector<uint8_t>{ 1, 2, 3, 4, 5 });
	msg::checkMsgCommunication(
	    message, std::make_shared<msg::Port<uint8_t>>(ports.first),
	    std::make_shared<msg::Port<uint8_t>>(ports.second));
}

void
testMsgSerialCommunicator() {
	testMsgSerialCommunication();
}