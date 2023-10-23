#include <msg/test/BaseMsgTests.hpp>
#include <msg/test/DualPort.hpp>

void
testMsgHandling() {
	msg::DualPort<uint8_t> dualPort;
	msg::Stream<uint8_t> &portA = dualPort.GetPortA();
	msg::Stream<uint8_t> &portB = dualPort.GetPortB();

	const msg::Msg message =
	    msg::Msg(0x5678, 5, std::vector<uint8_t>{ 1, 2, 3, 4, 5 });
	msg::checkMsgHandling(message, portA, portB);
}

void
testMsgHandler() {
	testMsgHandling();
}