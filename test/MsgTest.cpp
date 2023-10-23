
#include <assert.h>
#include <msg/Msg.hpp>
#include <numeric>
#include <stdint.h>
#include <vector>

void testConstructors();
void testSet();
void testRead();
void testWrite();
void testOperators();

void
testMsg() {
	testConstructors();
	testSet();
	testRead();
	testWrite();
	testOperators();
}

void
testConstructors() {
	uint16_t type = 0xABCD;
	uint16_t length = 25;
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg;

	data.resize(length);
	std::iota(data.begin(), data.end(), static_cast<uint8_t>(1));
	bytes.insert(bytes.end(), {
				      static_cast<uint8_t>(type & 0xff),
				      static_cast<uint8_t>(type >> 8),
				      static_cast<uint8_t>(length & 0xff),
				      static_cast<uint8_t>(length >> 8),
				  });
	bytes.insert(bytes.end(), data.begin(), data.end());

	msg = msg::Msg();
	assert(msg.type() == msg::MsgType::ERR);
	assert(msg.length() == 0);
	assert(msg.data().empty());

	msg = msg::Msg(type);
	assert(msg.type() == type);
	assert(msg.length() == 0);
	assert(msg.data().empty());

	msg = msg::Msg(type, length);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == std::vector<uint8_t>(length, 0));

	msg = msg::Msg(type, length, data);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg(bytes);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg(bytes.begin(), bytes.end());
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);
}

void
testSet() {
	uint16_t type = 0xABCD;
	uint16_t length = 25;
	std::vector<uint8_t> data = std::vector<uint8_t>();
	msg::Msg msg;

	data.resize(length);
	std::iota(data.begin(), data.end(), static_cast<uint8_t>(1));

	msg = msg::Msg();
	msg.setType(type);
	assert(msg.type() == type);
	assert(msg.length() == 0);
	assert(msg.data().empty());

	msg = msg::Msg();
	msg.setData(data, length);
	assert(msg.type() == msg::MsgType::ERR);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg();
	msg.setData(data.begin(), data.end(), length);
	assert(msg.type() == msg::MsgType::ERR);
	assert(msg.length() == length);
	assert(msg.data() == data);
}
void
testRead() {
	uint16_t type = 0xABCD;
	uint16_t length = 25;
	std::vector<uint8_t> header = std::vector<uint8_t>();
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg;

	header = std::vector<uint8_t>{
		static_cast<uint8_t>(type & 0xff),
		static_cast<uint8_t>(type >> 8),
		static_cast<uint8_t>(length & 0xff),
		static_cast<uint8_t>(length >> 8),
	};
	data.resize(length);
	std::iota(data.begin(), data.end(), static_cast<uint8_t>(1));
	bytes.insert(bytes.end(), header.begin(), header.end());
	bytes.insert(bytes.end(), data.begin(), data.end());

	msg = msg::Msg();
	msg.readMsg(bytes);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg();
	msg.readMsg(bytes.begin(), bytes.end());
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg();
	msg.readHeader(header);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == std::vector<uint8_t>(length, 0));

	msg = msg::Msg();
	msg.readHeader(header.begin(), header.end());
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == std::vector<uint8_t>(length, 0));

	msg = msg::Msg(header);
	msg.readData(data);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg(type, length);
	msg.readData(data);
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);

	msg = msg::Msg(type, length);
	msg.readData(data.begin(), data.end());
	assert(msg.type() == type);
	assert(msg.length() == length);
	assert(msg.data() == data);
}
void
testWrite() {
	uint16_t type = 0xABCD;
	uint16_t length = 25;
	std::vector<uint8_t> header = std::vector<uint8_t>();
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg;

	header = std::vector<uint8_t>{
		static_cast<uint8_t>(type & 0xff),
		static_cast<uint8_t>(type >> 8),
		static_cast<uint8_t>(length & 0xff),
		static_cast<uint8_t>(length >> 8),
	};
	data.resize(length);
	std::iota(data.begin(), data.end(), static_cast<uint8_t>(1));
	bytes.insert(bytes.end(), header.begin(), header.end());
	bytes.insert(bytes.end(), data.begin(), data.end());

	msg = msg::Msg(type, length, data);

	assert(msg.writeMsg() == bytes);
	assert(msg.writeHeader() == header);
	assert(msg.writeData() == data);
}
void
testOperators() {
	uint16_t type = 0xABCD;
	uint16_t length = 25;
	std::vector<uint8_t> header = std::vector<uint8_t>();
	std::vector<uint8_t> data = std::vector<uint8_t>();
	std::vector<uint8_t> bytes = std::vector<uint8_t>();
	msg::Msg msg1;
	msg::Msg msg2;

	header = std::vector<uint8_t>{
		static_cast<uint8_t>(type & 0xff),
		static_cast<uint8_t>(type >> 8),
		static_cast<uint8_t>(length & 0xff),
		static_cast<uint8_t>(length >> 8),
	};
	data.resize(length);
	std::iota(data.begin(), data.end(), static_cast<uint8_t>(1));
	bytes.insert(bytes.end(), header.begin(), header.end());
	bytes.insert(bytes.end(), data.begin(), data.end());

	msg1 = msg::Msg(type, length, data);
	assert(msg1 == bytes);

	msg1 = msg::Msg(bytes);
	msg2 = msg::Msg(type, length, data);
	assert(msg1 == msg2);
}