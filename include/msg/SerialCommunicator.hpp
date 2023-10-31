#ifndef _MSG_SERIAL_COMMUNICATOR_H
#define _MSG_SERIAL_COMMUNICATOR_H

#include "msg/Msg.hpp"
#include "msg/MsgHandler.hpp"
#include "msg/stream/Stream.hpp"
#include "msg/types.hpp"
#include <chrono>
#include <future>
#include <queue>
namespace msg {

class SerialCommunicator {
public:
	class Request;
	enum class RequestPriority : unsigned int {
		CRITICAL = 0,
		HIGH = 1,
		NORMAL = 2,
		LOW = 3
	};
	class WorkerThread;

	SerialCommunicator(std::unique_ptr<Serial<uint8_t>> s);
	~SerialCommunicator();

	void setDevice(std::unique_ptr<Serial<uint8_t>> s);
	// releasing the device makes every communication
	// operation except setDevice invalid
	std::unique_ptr<Serial<uint8_t>> releaseDevice();

	std::future<Msg>
	exchange(const Msg &msg,
	         RequestPriority priority = RequestPriority::NORMAL);
	std::future<void>
	transmit(const Msg &msg,
	         RequestPriority priority = RequestPriority::NORMAL);
	std::future<Msg>
	receive(RequestPriority priority = RequestPriority::NORMAL);

	std::map<timeStamp, Msg> receiveLog;
	std::chrono::milliseconds listeningInterval =
	    std::chrono::milliseconds(10);

protected:
	std::unique_ptr<Serial<uint8_t>> serial;
	MsgHandler handler;
	std::mutex serialMtx;

	std::unique_ptr<WorkerThread> worker;
	std::thread listeningThread;
	std::atomic<bool> listening{ false };
	std::condition_variable listeningCondition;

	void listen();
	void start();
	void stop();
};
} // namespace msg

#endif