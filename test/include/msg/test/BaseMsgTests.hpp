#ifndef _MSG_BASE_TESTS_HPP
#define _MSG_BASE_TESTS_HPP

#include <assert.h>
#include <future>
#include <msg/MsgHandler.hpp>

namespace msg {
inline void
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
	assert(receivedMsg == (msg::Msg)message);
}
} // namespace msg
#endif