/* See LICENSE file for copyright and license details. */
#ifndef _MSG_HPP
#define _MSG_HPP

#include <map>
#include <memory>
#include <msg/MsgType.hpp>
#include <stdint.h>
#include <string>
#include <vector>

/* Type Definitions */
namespace msg {
class Msg {
	// Assumes little-endianess
	using byteArray = std::vector<uint8_t>;
	using byteConstIter = byteArray::const_iterator;

public:
	static constexpr size_t sizeOfType = sizeof(MsgType::type);
	static constexpr size_t sizeOfLength = sizeof(uint16_t);
	static constexpr size_t sizeOfId = sizeof(uint16_t);
	static constexpr size_t sizeOfHeader =
	    sizeOfType + sizeOfLength + sizeOfId;

	Msg();
	Msg(uint16_t type);
	Msg(uint16_t type, uint16_t id);
	Msg(uint16_t type, uint16_t id, uint16_t length);
	Msg(uint16_t type, uint16_t length, uint16_t id, const byteArray &data);
	Msg(const byteArray &bytes);
	Msg(byteConstIter, byteConstIter);

	uint16_t type() const;
	uint16_t id() const;
	uint16_t length() const;
	const byteArray &data() const;

	void setType(uint16_t type);
	void setId(uint16_t id);
	void setData(const byteArray &, uint16_t length);
	void setData(byteConstIter, byteConstIter, uint16_t length);

	// Array's & Iterator's layout and size must match Msg specifications
	void readMsg(const byteArray &);
	void readMsg(byteConstIter, byteConstIter);
	void readHeader(const byteArray &);
	void readHeader(byteConstIter, byteConstIter);
	void readData(const byteArray &);
	void readData(byteConstIter, byteConstIter);

	byteArray writeMsg() const;
	byteArray writeHeader() const;
	byteArray writeData() const;

	operator byteArray() const;
	bool operator==(const Msg &other) const;

private:
	uint16_t _type;
	uint16_t _id;
	byteArray _data;
};
} // namespace msg

#endif