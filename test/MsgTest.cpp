
#include "msg/Msg.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <numeric>
#include <stdint.h>
#include <vector>

TEST_CASE("Msg Constructor", "[Msg]") {
	uint16_t type = msg::MsgType::UNDEF;
	uint16_t length = 0;
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg;

	SECTION("Initialize an Empty Msg") {}

	SECTION("Initialize Type") {
		auto t = GENERATE(msg::MsgType::ERR, msg::MsgType::DEBUG, 1384,
		                  0xABCD);
		type = t;

		msg = msg::Msg(type);
	}

	SECTION("Fully Initialize") {
		auto i = GENERATE(0, 1, 10);
		length = i;
		data.resize(i);

		SECTION("Initialize Type & Length") {
			msg = msg::Msg(type, length);
		}

		SECTION("Initialize Type, Length & Data") {
			std::iota(data.begin(), data.end(), 1);

			SECTION("Initialize Seperately") {
				msg = msg::Msg(type, length, data);
			}

			SECTION("Initialize Together") {
				bytes.insert(
				    bytes.end(),
				    {
					static_cast<uint8_t>(type & 0xff),
					static_cast<uint8_t>(type >> 8),
					static_cast<uint8_t>(length & 0xff),
					static_cast<uint8_t>(length >> 8),
				    });
				bytes.insert(bytes.end(), data.begin(),
				             data.end());

				SECTION("Initialize by Vector") {
					msg = msg::Msg(bytes);
				}
				SECTION("Initialize by Iters") {
					msg = msg::Msg(bytes.begin(),
					               bytes.end());
				}
			}
		}
	}

	REQUIRE(msg.checkInvariants());
	REQUIRE(msg.type() == type);
	REQUIRE(msg.length() == length);
	REQUIRE(msg.data().size() == length);
	REQUIRE(msg.data() == data);
}

TEST_CASE("Msg Get Header", "[Msg]") {
	auto type =
	    GENERATE(msg::MsgType::UNDEF, msg::MsgType::ACK, msg::MsgType::ERR);
	auto length = GENERATE(0, 1, 10);
	msg::Msg msg = msg::Msg(type, length);

	auto header = msg.header();
	REQUIRE(header.size() == 4);
	uint16_t readType = (header[1] << 8) | (header[0]);
	uint16_t readLength = (header[3] << 8) | (header[2]);
	REQUIRE(msg.checkInvariants());
	REQUIRE(type == readType);
	REQUIRE(length == readLength);
}

TEST_CASE("Msg Get Data", "[Msg]") {
	uint16_t type = msg::MsgType::UNDEF;
	auto length = GENERATE(0, 1, 10);
	std::vector<uint8_t> data = std::vector<uint8_t>();
	data.resize(length);
	std::iota(data.begin(), data.end(), 1);
	msg::Msg msg = msg::Msg(type, length, data);

	auto readData = msg.data();
	REQUIRE(msg.checkInvariants());
	REQUIRE(readData.size() == msg.length());
	REQUIRE(readData == data);
}

TEST_CASE("Msg Set Header", "[Msg]") {
	uint16_t type = msg::MsgType::UNDEF;
	uint16_t length = 0;
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg;

	SECTION("Assign Type") {
		auto t = GENERATE(msg::MsgType::ERR, msg::MsgType::DEBUG, 1384,
		                  0xABCD);
		type = t;
		msg.setHeader(type, length);
	}

	SECTION("Assign Length") {
		type = msg::MsgType::CONTINUE;
		auto i = GENERATE(0, 1, 10);
		length = i;

		SECTION("Assign by parameters") { msg.setHeader(type, length); }
		SECTION("Assign by one parameter") {
			bytes.insert(bytes.end(),
			             {
					 static_cast<uint8_t>(type & 0xff),
					 static_cast<uint8_t>(type >> 8),
					 static_cast<uint8_t>(length & 0xff),
					 static_cast<uint8_t>(length >> 8),
				     });

			SECTION("Assign by Bytes") { msg.setHeader(bytes); }
			SECTION("Assign by Iters") {
				msg.setHeader(bytes.begin(), bytes.end());
			}
		}

		data.resize(i);
	}

	REQUIRE(msg.checkInvariants());
	REQUIRE(msg.type() == type);
	REQUIRE(msg.length() == length);
}

TEST_CASE("Msg Set Data", "[Msg]") {
	uint16_t type = msg::MsgType::UNDEF;
	uint16_t length = 0;
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg = msg::Msg(msg::MsgType::ACK);

	SECTION("Assign Data") {
		auto i = GENERATE(0, 1, 10);
		length = i;
		msg.setHeader(type, length);
		data.resize(i);
		std::iota(data.begin(), data.end(), 1);

		SECTION("Assign by Vector") { msg.setData(data); }
		SECTION("Assign by Iters") {
			msg.setData(data.begin(), data.end());
		}
	}

	REQUIRE(msg.checkInvariants());
	REQUIRE(msg.data() == data);
}