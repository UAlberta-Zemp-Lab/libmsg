/* See LICENSE file for copyright and license details. */
#ifndef _MSG_HANDLER_HPP
#define _MSG_HANDLER_HPP

#include <memory>
#include <msg/Msg.hpp>
#include <msg/Stream.hpp>

namespace msg {
class MsgHandler {
public:
	Msg readMsg(Stream<uint8_t> &);
	void writeMsg(const Msg &, Stream<uint8_t> &);

	unsigned int retries = 0;

protected:
	uint16_t checkResponse(Stream<uint8_t> &);
};
} // namespace msg

#endif