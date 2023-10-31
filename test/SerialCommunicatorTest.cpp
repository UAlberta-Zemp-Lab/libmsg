#include "MsgBaseTests.cpp"
#include "util/DualPort.cpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

TEST_CASE("Message Communciation", "[SerialCommunicator]") {
	msg::DualPort<uint8_t> dualPort;
	std::unique_ptr<msg::Serial<uint8_t>> portA =
	    std::make_unique<msg::DualPort<uint8_t>::Port<uint8_t>>(
		std::move(dualPort.GetPortA()));
	std::unique_ptr<msg::Serial<uint8_t>> portB =
	    std::make_unique<msg::DualPort<uint8_t>::Port<uint8_t>>(
		std::move(dualPort.GetPortB()));
	const msg::Msg message =
	    GENERATE(msg::Msg(), msg::Msg(msg::MsgType::DEBUG),
	             msg::Msg(msg::MsgType::UNDEF, 5,
	                      std::vector<uint8_t>{ 1, 2, 3, 4, 5 }));
	msg::checkMsgCommunication(message, portA, portB);
}