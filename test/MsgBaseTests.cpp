#include "msg/MsgHandler.hpp"
#include <catch2/catch_test_macros.hpp>
#include <future>

namespace msg {
void
checkMsgHandling(const Msg &message, Stream<uint8_t> &portA,
                 Stream<uint8_t> &portB) {
	msg::Msg receivedMsg;

	std::future<void> write = std::async(
	    msg::MsgHandler::writeMsg, std::ref(message), std::ref(portA));
	std::future<msg::Msg> read =
	    std::async(msg::MsgHandler::readMsg, std::ref(portB));
	write.wait();
	read.wait();
	receivedMsg = read.get();
	REQUIRE(receivedMsg == (msg::Msg)message);
}
} // namespace msg