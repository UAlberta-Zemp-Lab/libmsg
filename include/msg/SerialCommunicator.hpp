#ifndef _MSG_SERIAL_COMMUNICATOR_HPP
#define _MSG_SERIAL_COMMUNICATOR_HPP

#include <chrono>
#include <future>
#include <msg/Msg.hpp>
#include <msg/MsgHandler.hpp>
#include <msg/MsgLog.hpp>
#include <msg/Stream.hpp>
#include <queue>
#include <vector>

namespace msg {

class SerialCommunicator {
	using timeStamp =
	    std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
	class Request;
	enum class RequestPriority : unsigned int {
		CRITICAL = 0,
		HIGH = 1,
		NORMAL = 2,
		LOW = 3
	};
	class WorkerThread;

	SerialCommunicator();
	SerialCommunicator(std::shared_ptr<Serial<uint8_t>> s);
	~SerialCommunicator();

	void setDevice(std::shared_ptr<Serial<uint8_t>> s);
	std::shared_ptr<Serial<uint8_t>> releaseDevice();
	bool isConnected() const;

	// Only valid while a device is connected
	std::future<Msg>
	exchange(const Msg &msg,
	         RequestPriority priority = RequestPriority::NORMAL);
	std::future<void>
	transmit(const Msg &msg,
	         RequestPriority priority = RequestPriority::NORMAL);
	std::future<Msg>
	receive(RequestPriority priority = RequestPriority::NORMAL);

	const std::shared_ptr<MsgLog> receiveLog = std::make_shared<MsgLog>();

	std::chrono::milliseconds listeningInterval =
	    std::chrono::milliseconds(10);

protected:
	std::shared_ptr<Serial<uint8_t>> serial;
	std::mutex serialMtx;

	std::unique_ptr<WorkerThread> worker;
	std::thread listeningThread;
	std::atomic<bool> listening{ false };
	std::condition_variable listeningCondition;
	std::condition_variable receiveCondition;
	bool running = false;

	void processRequest(Request);
	void listen();
	void start();
	void stop();
};
} // namespace msg

#endif