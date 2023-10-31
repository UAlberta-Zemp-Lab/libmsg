#include "MsgBaseTests.cpp"
#include "util/DualPort.cpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

TEST_CASE("Message Communciation", "[MsgHandler]") {
	msg::DualPort<uint8_t> dualPort;
	msg::Stream<uint8_t> &portA = dualPort.GetPortA();
	msg::Stream<uint8_t> &portB = dualPort.GetPortB();

	const msg::Msg message =
	    GENERATE(msg::Msg(), msg::Msg(msg::MsgType::DEBUG),
	             msg::Msg(msg::MsgType::UNDEF, 5,
	                      std::vector<uint8_t>{ 1, 2, 3, 4, 5 }));
	msg::checkMsgHandling(message, portA, portB);
}