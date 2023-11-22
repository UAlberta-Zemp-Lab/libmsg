#ifndef _MSG_COMMUNICATOR_REQUEST_HPP
#define _MSG_COMMUNICATOR_REQUEST_HPP
#include <future>
#include <msg/Msg.hpp>
#include <msg/SerialCommunicator.hpp>
#include <optional>

using timeStamp = std::chrono::time_point<std::chrono::high_resolution_clock>;
namespace msg {
struct SerialCommunicator::Request {

	std::optional<Msg> transmitMsg = std::nullopt;
	std::optional<std::promise<void>> transmitPromise = std::nullopt;
	std::optional<std::promise<Msg>> receivePromise = std::nullopt;
	RequestPriority priority = RequestPriority::NORMAL;
	timeStamp timestamp = std::chrono::high_resolution_clock::now();

	Request(Msg msg, std::promise<Msg> exchangePromise,
	        RequestPriority priority = RequestPriority::NORMAL)
	    : transmitMsg(msg), receivePromise(std::move(exchangePromise)),
	      priority(priority){};
	Request(Msg msg, std::promise<void> transmitPromise,
	        RequestPriority priority = RequestPriority::NORMAL)
	    : transmitMsg(msg), transmitPromise(std::move(transmitPromise)),
	      priority(priority){};
	Request(std::promise<Msg> receivePromise,
	        RequestPriority priority = RequestPriority::NORMAL)
	    : receivePromise(std::move(receivePromise)), priority(priority){};
	Request(){};
	Request(Request &) = delete;
	Request(Request &&) = default;
	Request &operator=(Request &&) = default;
	bool operator<(const Request &rhs) const {
		return priority > rhs.priority || timestamp > rhs.timestamp;
	}
};
} // namespace msg
#endif