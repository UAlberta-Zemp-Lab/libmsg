/* See LICENSE file for copyright and license details. */
#include "msg/Msg.hpp"
#include <iterator>
#include <memory>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace msg {
// Msg
Msg::Msg(byteConstIter begin, byteConstIter end) {
	this->setHeader(begin, end);
	if (this->length() > 0
	    && std::distance(begin, end) > this->sizeOfHeader) {
		this->setData(begin + this->sizeOfHeader, end);
	}
}
Msg::Msg(uint16_t type, uint16_t length, const byteArray &data) {
	this->setHeader(type, length);
	this->setData(data);
}
Msg::Msg(const byteArray &bytes) : Msg(bytes.begin(), bytes.end()) {}
Msg::Msg(uint16_t type, uint16_t length) { this->setHeader(type, length); }
Msg::Msg(uint16_t type) : Msg::Msg(type, 0) {}
Msg::Msg() : Msg::Msg(MsgType::UNDEF, 0) {}

std::unique_ptr<Msg>
Msg::clone() const {
	return std::make_unique<Msg>(*this);
}

uint16_t
Msg::type() const {
	uint16_t type = (this->_header[1] << 8) | (this->_header[0]);
	return type;
}

uint16_t
Msg::length() const {
	uint16_t length = (this->_header[3] << 8) | (this->_header[2]);
	return length;
}

unsigned int
Msg::size() const {
	return this->sizeOfHeader + this->_data.size();
}

const byteArray &
Msg::header() const {
	return this->_header;
}

const byteArray &
Msg::data() const {
	return this->_data;
}

void
Msg::setHeader(const byteConstIter &begin, const byteConstIter &end) {
	if (std::distance(begin, end) >= this->sizeOfHeader)
		this->_header.assign(begin, begin + this->sizeOfHeader);
	else if (std::distance(begin, end) >= this->sizeOfType) {
		this->_header.assign(begin, begin + this->sizeOfType);
		this->_header.push_back(0);
		this->_header.push_back(0);
	} else
		throw new std::runtime_error(
		    "Range too short to construct header");

	this->_data.resize(this->length());
}

void
Msg::setHeader(const byteArray &vector) {
	this->setHeader(vector.begin(), vector.end());
}

void
Msg::setHeader(uint16_t type, uint16_t length) {
	this->setHeader(byteArray{
	    static_cast<uint8_t>(type & 0xff),
	    static_cast<uint8_t>(type >> 8),
	    static_cast<uint8_t>(length & 0xff),
	    static_cast<uint8_t>(length >> 8),
	});
}

void
Msg::setData(const byteConstIter begin, const byteConstIter end) {
	if (std::distance(begin, end) != this->length()) {
		throw std::runtime_error("Data not correct size");
	}

	this->_data.assign(begin, end);
}

void
Msg::setData(const byteArray &bytes) {
	this->setData(bytes.begin(), bytes.end());
}

bool
Msg::checkInvariants() {
	bool invariant = true;
	if (this->length() != this->_data.size())
		invariant = false;
	return invariant;
}

Msg::operator byteArray() const {
	std::vector<uint8_t> output;
	output.reserve(this->size());
	output.insert(output.end(), _header.begin(), _header.end());
	output.insert(output.end(), _data.begin(), _data.end());
	return output;
}
} // namespace msg

bool
msg::Msg::operator==(const Msg &other) const {
	return (this == &other)
	       || ((this->_header == other._header)
	           && (this->_data == other._data));
}
