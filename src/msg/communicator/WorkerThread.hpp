#include "../util/priorityQueue.cpp"
#include "Request.hpp"
#include "msg/Msg.hpp"
#include "msg/SerialCommunicator.hpp"
#include "msg/types.hpp"
#include <future>
#include <iostream>

namespace msg {
struct SerialCommunicator::WorkerThread {

	WorkerThread(Serial<uint8_t> &serial, std::mutex &serialMtx)
	    : serial(serial), serialMtx(serialMtx){};
	void start();
	void queue(Request);
	void stop();
	bool busy();
	

	WorkerThread(WorkerThread &) = delete;
	WorkerThread(WorkerThread &&other)
	    : WorkerThread(std::move(other), other.serial, other.serialMtx){};
	WorkerThread(WorkerThread &&other, Serial<uint8_t> &serial,
	             std::mutex &serialMtx)
	    : serial(serial), serialMtx(serialMtx) {
		other.stop();
		requests = std::move(other.requests);
	};
	~WorkerThread();

private:
	void loop();
	void processRequest(Request);
	Serial<uint8_t> &serial;
	std::mutex &serialMtx;
	std::mutex queueMtx;
	priorityQueue<Request> requests;
	std::thread thread;
	std::condition_variable queueCondition;
	bool terminate = false;
};
} // namespace msg
