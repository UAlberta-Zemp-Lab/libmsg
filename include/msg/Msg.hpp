/* See LICENSE file for copyright and license details. */
#ifndef _MSG_H
#define _MSG_H

#include "msg/types.hpp"
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

/* Type Definitions */
namespace msg {

/* Reserved Message Types */
class MsgType {
public:
	uint16_t type;

	static inline constexpr uint16_t UNDEF = 0x0000;
	static inline constexpr uint16_t ERR = 0xFF00;
	static inline constexpr uint16_t ACK = 0xFF01;
	static inline constexpr uint16_t REJECT = 0xFF02;
	static inline constexpr uint16_t CONTINUE = 0xFF03;
	static inline constexpr uint16_t RETRY = 0xFF04;
	static inline constexpr uint16_t DEBUG = 0xFFFF;

	MsgType() : MsgType(MsgType::UNDEF){};
	MsgType(uint16_t type) : type(type){};

	operator uint16_t() const;
	virtual std::map<uint16_t, std::string> GetKnownMsgTypes();
	virtual bool IsKnownMsgType(uint16_t type);
};

class Msg {

public:
	Msg(byteConstIter, byteConstIter);
	// Assumes little-endianess
	Msg(const byteArray &bytes);
	Msg(uint16_t type, uint16_t length, const byteArray &data);
	Msg(uint16_t type, uint16_t length);
	Msg(uint16_t type);
	Msg();

	virtual std::unique_ptr<Msg> clone() const;

	static constexpr unsigned int sizeOfHeader = 4;
	static constexpr unsigned int sizeOfType =
	    sizeof(MsgType::type) / sizeof(uint8_t);
	static constexpr unsigned int sizeOfLength = 2;

	virtual uint16_t type() const;
	virtual uint16_t length() const;
	virtual unsigned int size() const;
	virtual const byteArray &header() const;
	virtual const byteArray &data() const;
	virtual void setHeader(uint16_t type, uint16_t length);
	virtual void setHeader(const byteConstIter &, const byteConstIter &);
	// Assumes little-endianess
	virtual void setHeader(const byteArray &);
	virtual void setData(const byteConstIter, const byteConstIter);
	virtual void setData(const byteArray &);

	virtual bool checkInvariants();

	virtual operator byteArray() const;
	virtual bool operator==(const Msg &other) const;

private:
	byteArray _header = byteArray(Msg::sizeOfHeader, 0);
	byteArray _data;
};
} // namespace msg

#endif