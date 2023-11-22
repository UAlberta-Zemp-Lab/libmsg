/* See LICENSE file for copyright and license details. */
#ifndef _MSG_TYPE_HPP
#define _MSG_TYPE_HPP

#include <map>
#include <msg/msgtypes.h>
#include <string>

namespace msg {
/* Reserved Message Types */
struct MsgType {
	uint16_t type = 0x0000;

	static inline constexpr uint16_t ERR = MSG_ERR;
	static inline constexpr uint16_t ACK = MSG_ACK;
	static inline constexpr uint16_t CONTINUE = MSG_CONTINUE;
	static inline constexpr uint16_t RETRY = MSG_RETRY;
	static inline constexpr uint16_t STOP = MSG_STOP;
	static inline constexpr uint16_t DEBUG = MSG_DEBUG;

	MsgType(){};
	MsgType(uint16_t type) : type(type){};

	operator uint16_t() const { return this->type; }
	static const std::map<uint16_t, std::string> knownMsgTypes;
	static bool isKnownMsgType(uint16_t type) {
		return knownMsgTypes.find(type) != knownMsgTypes.end();
	};
};

inline const std::map<uint16_t, std::string> MsgType::knownMsgTypes =
    std::map<uint16_t, std::string>{
	    {
             { ERR, "ERR" },
             { ACK, "ACK" },
             { CONTINUE, "CONTINUE" },
             { RETRY, "RETRY" },
             { STOP, "STOP" },
             { DEBUG, "DEBUG" },
	     }
};
} // namespace msg
#endif