/* See LICENSE file for copyright and license details. */
#ifndef _MSG_HANDLER_HPP
#define _MSG_HANDLER_HPP

#include <memory>
#include <msg/Msg.hpp>
#include <msg/Stream.hpp>

namespace msg {
class MsgHandler {
public:
	static Msg readMsg(Stream<uint8_t> &);
	static void writeMsg(const Msg &, Stream<uint8_t> &);

	static unsigned int retries;

protected:
	static uint16_t checkResponse(Stream<uint8_t> &);
};
} // namespace msg

#endif