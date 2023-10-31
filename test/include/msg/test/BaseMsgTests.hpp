#ifndef _MSG_BASE_TESTS_H
#define _MSG_BASE_TESTS_H

#include "msg/MsgHandler.hpp"
#include "msg/SerialCommunicator.hpp"
#include <catch2/catch_test_macros.hpp>
#include <future>

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
	REQUIRE(receivedMsg == (msg::Msg)message);
}

inline void
checkMsgCommunication(const Msg &message,
                      std::unique_ptr<Serial<uint8_t>> &portA,
                      std::unique_ptr<Serial<uint8_t>> &portB) {
	msg::Msg receivedMsg;

	msg::SerialCommunicator commA(std::move(portA));
	msg::SerialCommunicator commB(std::move(portB));

	SECTION("No Communication") {
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
	}

	SECTION("Transmit & Receive") {
		auto receiveFuture = commB.receive();
		commA.transmit(message);
		receivedMsg = receiveFuture.get();
		REQUIRE(receivedMsg == (msg::Msg)message);

		receiveFuture = commA.receive();
		commB.transmit(receivedMsg);
		receivedMsg = receiveFuture.get();
		REQUIRE(receivedMsg == (msg::Msg)message);
	}

	SECTION("Exchange") {
		auto receiveFuture = commB.receive();
		auto exchangedFuture = commA.exchange(message);
		receivedMsg = receiveFuture.get();
		REQUIRE(receivedMsg == (msg::Msg)message);
		commB.transmit(receivedMsg);
		receivedMsg = exchangedFuture.get();
		REQUIRE(receivedMsg == (msg::Msg)message);
	}

	SECTION("Transmit & Receive by Listening") {
		commA.transmit(message);
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		REQUIRE(commB.receiveLog.size() == 1);
		receivedMsg = commB.receiveLog.begin()->second;
		REQUIRE(receivedMsg == (msg::Msg)message);

		commB.transmit(receivedMsg);
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		receivedMsg = commA.receiveLog.begin()->second;
		REQUIRE(receivedMsg == (msg::Msg)message);
	}

	portA = commA.releaseDevice();
	portB = commB.releaseDevice();
}
} // namespace msg

#endif