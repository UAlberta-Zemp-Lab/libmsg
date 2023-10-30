/* See LICENSE file for copyright and license details. */
#include "msg/MsgHandler.hpp"
#include <mutex>
#include <stdbool.h>
#include <stdexcept>
#include <stdint.h>
#include <unistd.h>
#include <vector>

/*
 * Attempts to read a message using the current stream.  Will throw with
 * appropriate message on failure
 */
msg::Msg
msg::MsgHandler::readMsg(Stream<uint8_t> &stream) {

	Msg readMsg;
	while (true) {
		try {
			readMsg = Msg(stream.read(4));
		} catch (std::bad_cast &x) { // TODO: change to a custom type
			                     // for rejected Msgs
			stream << Msg(MsgType::REJECT);
			throw x;
		} catch (std::exception &x) {
			stream << Msg(MsgType::RETRY);
			continue;
		}

		if (readMsg.length() > 0) {
			try {
				stream << Msg(MsgType::CONTINUE);
				readMsg.setData(stream.read(readMsg.length()));
			} catch (std::bad_alloc &x) {
				stream << Msg(MsgType::ERR);
				throw x;
			} catch (std::exception &x) {
				stream << Msg(MsgType::RETRY);
				continue;
			}
		}

		stream << Msg(MsgType::ACK);
		return readMsg;
	}
}

/*
 * Attempts to write a message using the current stream.  Will throw with
 * appropriate message on failure.
 */
void
msg::MsgHandler::writeMsg(const msg::Msg &writeMsg, Stream<uint8_t> &stream) {
	while (true) {
		stream << writeMsg.header();

		switch (MsgHandler::checkResponse(stream)) {
		case MsgType::ACK:
			return;
		case MsgType::CONTINUE:
			break;
		case MsgType::RETRY:
			continue;
		case MsgType::REJECT:
			throw std::runtime_error(
			    "Message Rejected"); // Add custom exceptions
		case MsgType::ERR:
			throw std::runtime_error("Message Error");
		default:
			throw std::runtime_error("Unknown Response Type");
		}

		stream << writeMsg.data();
		switch (MsgHandler::checkResponse(stream)) {
		case MsgType::ACK:
			return;
		case MsgType::RETRY:
			continue;
		case MsgType::ERR:
			throw std::runtime_error("Message Error");
		default:
			throw std::runtime_error("Unknown Response Type");
		}
	}
}

uint16_t
msg::MsgHandler::checkResponse(Stream<uint8_t> &stream) {
	std::vector<uint8_t> response(4, 0);
	stream >> response;
	uint16_t type = (response[1] << 8) | response[0];
	return type;
}