#include "msg/SerialCommunicator.hpp"
#include "Request.hpp"
#include "WorkerThread.hpp"
#include <future>
#include <mutex>

namespace msg {
SerialCommunicator::SerialCommunicator(std::unique_ptr<Serial<uint8_t>> s)
    : serial(std::move(s)),
      worker(std::make_unique<WorkerThread>(*serial, serialMtx)) {
	start();
};

SerialCommunicator::~SerialCommunicator() {

	try {
		stop();
	} catch (const std::exception &x) {
		std::cerr << "msg::MsgCommunicator Destuctor Failed "
			  << x.what() << std::endl;
	}
}

void
SerialCommunicator::setDevice(std::unique_ptr<Serial<uint8_t>> s) {
	stop();
	serial = std::move(s);
	worker =
	    std::make_unique<WorkerThread>(std::move(*worker), *s, serialMtx);
	start();
}

std::unique_ptr<Serial<uint8_t>>
SerialCommunicator::releaseDevice() {
	stop();
	std::unique_ptr<Serial<uint8_t>> ptr = nullptr;
	swap(ptr, serial);
	return ptr;
}

std::future<Msg>
SerialCommunicator::exchange(const Msg &msg, RequestPriority priority) {
	std::promise<Msg> exchangePromise;
	auto future = exchangePromise.get_future();

	Request request(msg, std::move(exchangePromise), priority);
	this->worker->queue(std::move(request));

	return future;
}

std::future<void>
SerialCommunicator::transmit(const Msg &msg,
                             SerialCommunicator::RequestPriority priority) {
	std::promise<void> transmitPromise;
	auto future = transmitPromise.get_future();

	Request request(msg, std::move(transmitPromise), priority);
	this->worker->queue(std::move(request));

	return future;
}

std::future<Msg>
SerialCommunicator::receive(RequestPriority priority) {
	std::promise<Msg> receivePromise;
	auto future = receivePromise.get_future();

	Request request(std::move(receivePromise), priority);
	this->worker->queue(std::move(request));

	return future;
}

void
SerialCommunicator::listen() {
	while (listening) {
		std::unique_lock<std::mutex> lock(serialMtx);
		listeningCondition.wait_for(lock, listeningInterval, [this] {
			return !listening || !worker->busy();
		});

		if (!listening) {
			return;
		}

		if (serial->numberOfBytesAvailable() >= Msg::sizeOfHeader) {
			msg::Msg receiveMsg = msg::MsgHandler::readMsg(*serial);
			receiveLog.insert(std::pair<timeStamp, Msg>(
			    std::chrono::high_resolution_clock::now(),
			    receiveMsg));
		}
	}
}
} // namespace msg

void
msg::SerialCommunicator::start() {
	worker->start();
	if (!listening) {
		{
			std::lock_guard<std::mutex> lock(serialMtx);
			this->listening = true;
		}
		listeningThread = std::thread(&listen, this);
	}
}

void
msg::SerialCommunicator::stop() {
	worker->stop();
	if (listening) {
		{
			std::lock_guard<std::mutex> lock(serialMtx);
			this->listening = false;
		}
		listeningCondition.notify_all();
		listeningThread.join();
	}
}
