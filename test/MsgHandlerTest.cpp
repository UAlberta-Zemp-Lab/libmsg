#include <msg/test/BaseMsgTests.hpp>
#include <msg/test/DualPort.hpp>

void
testMsgHandling() {
	auto ports = msg::Port<uint8_t>::GetPair();
	const msg::Msg message =
	    msg::Msg(0x5678, 5, std::vector<uint8_t>{ 1, 2, 3, 4, 5 });
	msg::checkMsgHandling(message, ports.first, ports.second);
}

void
testMsgHandler() {
	testMsgHandling();
}