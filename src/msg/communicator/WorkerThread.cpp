#include "WorkerThread.hpp"
#include "Request.hpp"

namespace msg {
using Request = SerialCommunicator::Request;
using RequestPriority = SerialCommunicator::RequestPriority;
using WorkerThread = SerialCommunicator::WorkerThread;

WorkerThread::~WorkerThread() {
	try {
		stop();
	} catch (const std::exception &x) {
		std::cerr << "msg::MsgCommunicator::WorkerThread "
			     "Destuctor Failed "
			  << x.what() << std::endl;
	}
}

void
WorkerThread::start() {
	if (terminate) {
		std::lock_guard<std::mutex> lock(queueMtx);
		terminate = false;
	}
	thread = std::thread(&WorkerThread::loop, this);
}

void
WorkerThread::queue(Request request) {
	{
		std::lock_guard<std::mutex> lock(queueMtx);
		requests.push(std::move(request));
	}
	queueCondition.notify_one();
}

void
WorkerThread::stop() {
	if (!terminate) {
		{
			std::lock_guard<std::mutex> lock(queueMtx);
			terminate = true;
		}
		queueCondition.notify_all();
		thread.join();
	}
}

bool
WorkerThread::busy() {
	bool isbusy;
	{
		std::unique_lock<std::mutex> lock(queueMtx, std::defer_lock);

		isbusy = !lock.try_lock() || !requests.empty();
	}
	return isbusy;
}

void
WorkerThread::loop() {
	while (true) {
		std::unique_lock<std::mutex> serialLock(serialMtx,
		                                        std::defer_lock);
		Request request;
		{

			std::unique_lock<std::mutex> queueLock(queueMtx);
			queueCondition.wait(queueLock, [this] {
				return !requests.empty() || terminate;
			});
			if (terminate) {
				return;
			}
			serialLock.lock();
			request = this->requests.pop();
		}

		processFunction(std::move(request));
	}
}
} // namespace msg
