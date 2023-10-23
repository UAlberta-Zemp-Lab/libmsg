/* See LICENSE file for copyright and license details. */
#ifndef _MSG_HANDLER_H
#define _MSG_HANDLER_H

#include "msg/Msg.hpp"
#include "msg/stream/Stream.hpp"
#include <memory>

namespace msg {
class MsgHandler {
public:
	MsgHandler(){};
	MsgHandler(std::unique_ptr<Stream<uint8_t>> s) : stream(std::move(s)){};
	Msg readMsg();
	void writeMsg(const Msg &);
	void attach(std::unique_ptr<Stream<uint8_t>> stream);
	void detach();

protected:
	std::unique_ptr<Stream<uint8_t>> stream;
	mutable std::unique_ptr<std::mutex> mtx =
	    std::make_unique<std::mutex>();
	uint16_t checkResponse();
};
} // namespace msg

#endif