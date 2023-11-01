/* See LICENSE file for copyright and license details. */
#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include "msg/Msg.hpp"
#include "msg/stream/Stream.hpp"
#include <memory>

namespace msg {
class MsgHandler {
public:
	static Msg readMsg(Stream<uint8_t> &,
	                   const MsgValidator & = MsgValidator());
	static void writeMsg(const Msg &, Stream<uint8_t> &);

protected:
	static uint16_t checkResponse(Stream<uint8_t> &);
};
} // namespace msg

#endif