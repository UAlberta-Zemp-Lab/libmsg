#ifndef _MSG_BASE_TESTS_HPP
#define _MSG_BASE_TESTS_HPP

#include <assert.h>
#include <future>
#include <msg/MsgHandler.hpp>
#include <msg/SerialCommunicator.hpp>

namespace msg {
inline void
checkMsgHandling(const Msg &message, Stream<uint8_t> &portA,
                 Stream<uint8_t> &portB) {
	msg::Msg receivedMsg;

	MsgHandler handler;
	std::future<void> write =
	    std::async(&msg::MsgHandler::writeMsg, std::ref(handler),
	               std::ref(message), std::ref(portA));
	std::future<msg::Msg> read = std::async(
	    &msg::MsgHandler::readMsg, std::ref(handler), std::ref(portB));
	write.wait();
	read.wait();
	receivedMsg = read.get();
	assert(receivedMsg == (msg::Msg)message);
}

inline void
checkMsgCommunication(const Msg &message,
                      std::shared_ptr<Serial<uint8_t>> portA,
                      std::shared_ptr<Serial<uint8_t>> portB) {
	msg::Msg receivedMsg;

	// Set And Release
	{

		msg::SerialCommunicator commA;
		msg::SerialCommunicator commB;

		commA.setDevice(portA);
		commB.setDevice(portB);

		std::this_thread::sleep_for(std::chrono::milliseconds(15));

		commA.releaseDevice();
		commB.releaseDevice();
	}

	// No Communication
	{

		msg::SerialCommunicator commA(portA);
		msg::SerialCommunicator commB(portB);

		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}

	// Transmit & Receive
	{
		msg::SerialCommunicator commA(portA);
		msg::SerialCommunicator commB(portB);
		auto receiveFuture = commB.receive();
		commA.transmit(message);
		receivedMsg = receiveFuture.get();
		assert(receivedMsg == (msg::Msg)message);

		receiveFuture = commA.receive();
		commB.transmit(receivedMsg);
		receivedMsg = receiveFuture.get();
		assert(receivedMsg == (msg::Msg)message);
	}

	// Exchange
	{
		msg::SerialCommunicator commA(portA);
		msg::SerialCommunicator commB(portB);
		auto receiveFuture = commB.receive();
		auto exchangedFuture = commA.exchange(message);
		receivedMsg = receiveFuture.get();
		assert(receivedMsg == (msg::Msg)message);
		commB.transmit(receivedMsg);
		receivedMsg = exchangedFuture.get();
		assert(receivedMsg == (msg::Msg)message);
	}

	// Transmit & Receive by Listening
	{
		msg::SerialCommunicator commA(portA);
		msg::SerialCommunicator commB(portB);
		commA.transmit(message);
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		assert(commB.receiveLog->size() == 1);
		receivedMsg = commB.receiveLog->pop().second;
		assert(receivedMsg == (msg::Msg)message);

		commB.transmit(receivedMsg);
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		receivedMsg = commA.receiveLog->pop().second;
		assert(receivedMsg == (msg::Msg)message);
	}
}
} // namespace msg

#endif