/* See LICENSE file for copyright and license details. */
#include <iostream>
#include <msg/MsgHandler.hpp>
#include <msg/unistd.h>
#include <mutex>
#include <sstream>
#include <stdbool.h>
#include <stdexcept>
#include <stdint.h>
#include <vector>

/*
 * Attempts to read a message using the current stream.  Will throw with
 * appropriate message on failure
 */
msg::Msg
msg::MsgHandler::readMsg(Stream<uint8_t> &stream) {

	Msg readMsg;
	for (unsigned int tries = retries + 1; tries; tries--) {
		try {
			readMsg = Msg(stream.read(Msg::sizeOfHeader));
		} catch (const std::exception &x) {
			stream << Msg((tries) ? MsgType::ERR : MsgType::RETRY);
			std::cerr << "Header Read Failed: " << x.what()
				  << std::endl;
			continue;
		}

		switch (readMsg.type()) {
		case MsgType::ACK:
		case MsgType::CONTINUE:
		case MsgType::RETRY:
			stream << Msg(MsgType::STOP);
			throw std::runtime_error("Received Basic Msg Type");
		case MsgType::STOP:
			throw std::runtime_error("Received Stop");
		default:
			break;
		}

		if (readMsg.length() > 0) {
			try {
				stream << Msg(MsgType::CONTINUE);
				readMsg.readData(stream.read(readMsg.length()));
			} catch (std::bad_alloc &) {
				stream << Msg(MsgType::ERR);
				throw;
			} catch (const std::exception &x) {
				std::cerr << "Data Read Failed: " << x.what()
					  << std::endl;
				stream << Msg((tries) ? MsgType::ERR
				                      : MsgType::RETRY);
				continue;
			}
		}

		stream << Msg(MsgType::ACK);
		if (tries <= MsgHandler::retries) {
			std::cerr << "Read Recovered: " << readMsg.type() << " "
				  << readMsg.length() << std::endl;
		}
		return readMsg;
	}

	throw std::runtime_error("Retry Count exceeded, could not read Msg");
}

/*
 * Attempts to write a message using the current stream.  Will throw with
 * appropriate message on failure.
 */
void
msg::MsgHandler::writeMsg(const msg::Msg &writeMsg, Stream<uint8_t> &stream) {
	for (unsigned int tries = retries + 1; tries; tries--) {
		stream << writeMsg.writeHeader();

		uint16_t response;
		try {
			response = MsgHandler::checkResponse(stream);
		} catch (const std::exception &x) {
			throw std::runtime_error(std::string("No response: ")
			                         + x.what());
		}
		switch (response) {
		case MsgType::ACK:
			return;
		case MsgType::CONTINUE:
			break;
		case MsgType::RETRY:
			continue;
		case MsgType::STOP:
			throw std::runtime_error("Received Message Stop");
		case MsgType::ERR:
			throw std::runtime_error("Message Error");
		default: {
			if (stream.options.WritePriority
			    == StreamOptions::WritePriority::Master) {
				continue;
			}
			throw std::runtime_error("sWrite");
		}
		}

		stream << writeMsg.data();
		try {
			response = MsgHandler::checkResponse(stream);
		} catch (const std::exception &x) {
			throw std::runtime_error(std::string("No response: ")
			                         + x.what());
		}
		switch (response) {
		case MsgType::ACK:
			return;
		case MsgType::RETRY:
			continue;
		case MsgType::STOP:
			throw std::runtime_error("Received Message Stop");
		case MsgType::ERR:
			throw std::runtime_error("Message Error");
		default:
			throw std::runtime_error("Unknown Response Type");
		}
	}

	throw std::runtime_error("Retry Count exceeded, could not write Msg");
}

uint16_t
msg::MsgHandler::checkResponse(Stream<uint8_t> &stream) {
	return Msg(stream.read(Msg::sizeOfHeader)).type();
}