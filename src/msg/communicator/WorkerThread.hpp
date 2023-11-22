#ifndef _MSG_COMMUNICATOR_WORKER_HPP
#define _MSG_COMMUNICATOR_WORKER_HPP

#include "../util/priorityQueue.cpp"
#include "Request.hpp"
#include <future>
#include <iostream>
#include <msg/Msg.hpp>
#include <msg/SerialCommunicator.hpp>

namespace msg {
struct SerialCommunicator::WorkerThread {

	WorkerThread(Serial<uint8_t> &serial, std::mutex &serialMtx,
	             std::function<void(Request)> processFunction)
	    : serial(serial), serialMtx(serialMtx),
	      processFunction(processFunction){};
	void start();
	void queue(Request);
	void stop();
	bool busy();

	WorkerThread(WorkerThread &) = delete;
	WorkerThread(WorkerThread &&other)
	    : WorkerThread(std::move(other), other.serial, other.serialMtx,
	                   other.processFunction){};
	WorkerThread(WorkerThread &&other, Serial<uint8_t> &serial,
	             std::mutex &serialMtx,
	             std::function<void(Request)> processFunction)
	    : WorkerThread(serial, serialMtx, processFunction) {
		if (other.running) {
			other.stop();
		}
		requests = std::move(other.requests);
	};
	~WorkerThread();

private:
	void loop();
	Serial<uint8_t> &serial;
	std::mutex &serialMtx;
	std::function<void(Request)> processFunction;
	std::mutex queueMtx;
	priorityQueue<Request> requests;
	std::thread thread;
	std::condition_variable queueCondition;
	bool terminate = false;
	bool running = false;
};
} // namespace msg

#endif