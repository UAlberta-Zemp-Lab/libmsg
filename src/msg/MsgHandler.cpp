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
msg::MsgHandler::readMsg(Stream<uint8_t> &stream,
                         const MsgValidator &validator) {

	Msg readMsg;
	while (true) {
		try {
			readMsg = Msg(stream.read(Msg::sizeOfHeader));
			validator.validate(readMsg);
		} catch (msg::MsgException &x) {
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
				validator.validate(readMsg);
			} catch (msg::MsgException &x) {
				stream << Msg(MsgType::REJECT);
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
		case MsgType::REJECT:
			throw std::runtime_error("Message Rejected");
		case MsgType::ERR:
			throw std::runtime_error("Message Error");
		default:
			throw std::runtime_error("Unknown Response Type");
		}
	}
}

uint16_t
msg::MsgHandler::checkResponse(Stream<uint8_t> &stream) {
	return Msg(stream.read(Msg::sizeOfHeader)).type();
}