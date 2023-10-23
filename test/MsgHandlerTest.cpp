#include "msg/MsgHandler.hpp"
#include "util/DualPort.cpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <future>
#include <memory>
#include <numeric>
#include <stdint.h>
#include <vector>

void writeMsg(std::unique_ptr<msg::Stream<uint8_t>> stream,
              const msg::Msg &msg);
void readMsg(std::unique_ptr<msg::Stream<uint8_t>> stream, msg::Msg &msg);

TEST_CASE("Initialize MsgHandler", "[MsgHandler]") {
	auto handler = msg::MsgHandler();
	SECTION("Default initialization") {}

	SECTION("Intiialization with Stream") {
		msg::DualPort<uint8_t> dualPort;
		auto portA =
		    std::make_unique<msg::DualPort<uint8_t>::Port<uint8_t>>(
			std::move(dualPort.GetPortA()));
		REQUIRE_NOTHROW(
		    [&]() { handler = msg::MsgHandler(std::move(portA)); }());
	}
}

TEST_CASE("Attach & Detach Streams from MsgHandler", "[MsgHandler]") {
	auto handler = msg::MsgHandler();
	msg::DualPort<uint8_t> dualPort;
	auto portA = std::make_unique<msg::DualPort<uint8_t>::Port<uint8_t>>(
	    std::move(dualPort.GetPortA()));
	REQUIRE_NOTHROW([&]() {
		handler.attach(std::move(portA));
		handler.detach();
	}());
}

TEST_CASE("Message Communciation", "[MsgHandler]") {
	msg::DualPort<uint8_t> dualPort;
	auto portA = std::make_unique<msg::DualPort<uint8_t>::Port<uint8_t>>(
	    std::move(dualPort.GetPortA()));
	auto portB = std::make_unique<msg::DualPort<uint8_t>::Port<uint8_t>>(
	    std::move(dualPort.GetPortB()));

	auto message =
	    GENERATE(msg::Msg(), msg::Msg(msg::MsgType::DEBUG),
	             msg::Msg(msg::MsgType::UNDEF, 5,
	                      std::vector<uint8_t>{ 1, 2, 3, 4, 5 }));
	msg::Msg receivedMsg;

	std::future<void> write =
	    std::async(writeMsg, std::move(portA), std::ref(message));
	std::future<void> read =
	    std::async(readMsg, std::move(portB), std::ref(receivedMsg));
	write.wait();
	read.wait();
	REQUIRE(receivedMsg == (msg::Msg)message);
}

void
writeMsg(std::unique_ptr<msg::Stream<uint8_t>> stream, const msg::Msg &msg) {
	msg::MsgHandler handler = msg::MsgHandler(std::move(stream));

	handler.writeMsg(msg);
}

void
readMsg(std::unique_ptr<msg::Stream<uint8_t>> stream, msg::Msg &msg) {
	msg::MsgHandler handler = msg::MsgHandler(std::move(stream));

	msg = handler.readMsg();
}