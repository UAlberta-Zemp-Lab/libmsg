/* See LICENSE file for copyright and license details. */
#include "msg/MsgHandler.hpp"
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
msg::MsgHandler::readMsg() {

	if (this->stream == NULL) {
		throw std::runtime_error("No Stream Connected");
	}

	std::lock_guard<std::mutex> lock(*(this->mtx));
	Stream<uint8_t> &stream = *(this->stream);

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
msg::MsgHandler::writeMsg(const msg::Msg &writeMsg) {

	if (this->stream == NULL) {
		throw std::runtime_error("No Stream Connected");
	}

	std::lock_guard<std::mutex> lock(*(this->mtx));
	Stream<uint8_t> &stream = *(this->stream);

	while (true) {
		stream << writeMsg.header();

		switch (this->checkResponse()) {
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
		switch (this->checkResponse()) {
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

void
msg::MsgHandler::attach(std::unique_ptr<Stream<uint8_t>> stream) {
	std::lock_guard<std::mutex> lock(*(this->mtx));
	this->stream = std::move(stream);
};
void
msg::MsgHandler::detach() {
	std::lock_guard<std::mutex> lock(*(this->mtx));
	this->stream == NULL;
}

uint16_t
msg::MsgHandler::checkResponse() {
	std::vector<uint8_t> response(4, 0);
	*(this->stream) >> response;
	uint16_t type = (response[1] << 8) | response[0];
	return type;
}
