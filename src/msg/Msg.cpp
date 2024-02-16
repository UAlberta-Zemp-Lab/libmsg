/* See LICENSE file for copyright and license details. */
#include <iterator>
#include <limits>
#include <memory>
#include <msg/Msg.hpp>
#include <stdbool.h>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <vector>

using byteArray = std::vector<uint8_t>;
using byteConstIter = byteArray::const_iterator;

namespace msg {
Msg::Msg() : Msg::Msg(MsgType::ERR) {}
Msg::Msg(uint16_t type) : Msg(type, 0) {}
Msg::Msg(uint16_t type, uint16_t length) : Msg(type, length, 0) {}
Msg::Msg(uint16_t type, uint16_t id, uint16_t length)
    : _type(type), _id(id), _data(byteArray(length, 0)) {}
Msg::Msg(uint16_t type, uint16_t id, uint16_t length, const byteArray &data)
    : Msg(type, id, 0) {
	setData(data, length);
}
Msg::Msg(const byteArray &bytes) : Msg(bytes.begin(), bytes.end()) {}
Msg::Msg(byteConstIter begin, byteConstIter end) : Msg() {
	readMsg(begin, end);
}

uint16_t
Msg::type() const {
	return _type;
}
uint16_t
Msg::id() const {
	return _id;
}
uint16_t
Msg::length() const {
	return static_cast<uint16_t>(_data.size());
}
const byteArray &
Msg::data() const {
	return _data;
}

void
Msg::setType(uint16_t type) {
	_type = type;
}
void
Msg::setId(uint16_t id) {
	_id = id;
}
void
Msg::setData(const byteArray &data, uint16_t length) {
	setData(data.begin(), data.end(), length);
}
void
Msg::setData(byteConstIter begin, byteConstIter end, uint16_t length) {
	if (std::distance(begin, end) < length) {
		throw std::runtime_error("Data not large enough");
	}

	_data.assign(begin, begin + length);
}

void
Msg::readMsg(const byteArray &bytes) {
	readMsg(bytes.begin(), bytes.end());
}

void
Msg::readMsg(byteConstIter begin, byteConstIter end) {

	this->readHeader(begin, end);
	if (length() > 0 && std::distance(begin + sizeOfHeader, end) != 0) {
		readData(begin + sizeOfHeader, end);
	}
}

void
Msg::readHeader(const byteArray &bytes) {
	readHeader(bytes.begin(), bytes.end());
}

void
Msg::readHeader(byteConstIter begin, byteConstIter end) {
	if (std::distance(begin, end) > 0
	    && static_cast<size_t>(std::distance(begin, end)) < sizeOfHeader) {
		throw std::runtime_error("Range too short to construct header");
	}

	uint16_t type = (*(begin + 1) << 8) | (*begin);
	std::advance(begin, 2);
	uint16_t id = (*(begin + 1) << 8) | (*begin);
	std::advance(begin, 2);
	uint16_t _length = (*(begin + 1) << 8) | (*begin);
	std::advance(begin, 2);

	_data.resize(_length);
	_type = type;
	_id = id;
}

void
Msg::readData(const byteArray &bytes) {
	readData(bytes.begin(), bytes.end());
}

void
Msg::readData(byteConstIter begin, byteConstIter end) {
	if (std::distance(begin, end) != length()) {
		throw std::runtime_error("Data not correct size");
	}

	_data.assign(begin, end);
}

byteArray
Msg::writeHeader() const {
	return byteArray{
		static_cast<uint8_t>(_type & 0xff),
		static_cast<uint8_t>(_type >> 8),
		static_cast<uint8_t>(_id & 0xff),
		static_cast<uint8_t>(_id >> 8),
		static_cast<uint8_t>(length() & 0xff),
		static_cast<uint8_t>(length() >> 8),
	};
}

byteArray
Msg::writeData() const {
	return data();
}

byteArray
Msg::writeMsg() const {
	byteArray msg = writeHeader();
	msg.reserve(sizeOfHeader + length());
	msg.insert(msg.end(), _data.begin(), _data.end());
	return msg;
}

Msg::operator byteArray() const { return writeMsg(); }
} // namespace msg

bool
msg::Msg::operator==(const Msg &other) const {
	return (this == &other)
	       || ((_type == other._type) && (_id == other._id)
	           && (_data == other._data));
}
