#include "msg/SerialCommunicator.hpp"
#include "Request.hpp"
#include "WorkerThread.hpp"
#include <future>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace msg {
SerialCommunicator::SerialCommunicator()
    : serial(nullptr), worker(std::make_unique<WorkerThread>(
			   *serial, serialMtx,
			   std::bind(&SerialCommunicator::processRequest, this,
                                     std::placeholders::_1))){};
SerialCommunicator::SerialCommunicator(std::shared_ptr<Serial<uint8_t>> s)
    : serial(std::move(s)), worker(std::make_unique<WorkerThread>(
				*serial, serialMtx,
				std::bind(&SerialCommunicator::processRequest,
                                          this, std::placeholders::_1))) {
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
SerialCommunicator::setDevice(std::shared_ptr<Serial<uint8_t>> s) {
	stop();
	serial = std::move(s);
	worker = std::make_unique<WorkerThread>(
	    std::move(*worker), *s, serialMtx,
	    std::bind(&SerialCommunicator::processRequest, this,
	              std::placeholders::_1));
	start();
}

std::shared_ptr<Serial<uint8_t>>
SerialCommunicator::releaseDevice() {
	stop();
	std::shared_ptr<Serial<uint8_t>> ptr = nullptr;
	swap(ptr, serial);
	return ptr;
}

bool
SerialCommunicator::isConnected() const {
	return serial && serial->isConnected();
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
			try {
				msg::MsgHandler handler;
				msg::Msg receiveMsg = handler.readMsg(*serial);
				receiveLog->push(std::pair<timeStamp, Msg>(
				    std::chrono::high_resolution_clock::now(),
				    receiveMsg));
			} catch (std::exception &x) {
				std::cerr << "msg::MsgCommunicator::listen "
					     "receive Failed: "
					  << x.what() << std::endl
					  << std::endl;
				serial->flushRead();
			}
		}
	}
}

void
SerialCommunicator::start() {
	if (!serial) {
		return;
	}
	if (running) {
		throw std::runtime_error("Started when running");
	}
	worker->start();
	if (!listening) {
		{
			std::lock_guard<std::mutex> lock(serialMtx);
			this->listening = true;
		}
		listeningThread =
		    std::thread(&SerialCommunicator::listen, this);
	}

	running = true;
}

void
SerialCommunicator::stop() {
	if (!serial) {
		return;
	}
	if (!running) {
		throw std::runtime_error("Stopped when not running");
	}
	worker->stop();
	if (listening) {
		{
			std::lock_guard<std::mutex> lock(serialMtx);
			this->listening = false;
		}
		listeningCondition.notify_all();
		listeningThread.join();
	}

	running = false;
}

void
SerialCommunicator::processRequest(Request request) {
	if (request.transmitMsg) {
		while (true) {
			try {
				while (serial->isDataAvailable()) {
					try {
						serial->flushWrite();
						MsgHandler handler;
						Msg receiveMsg =
						    handler.readMsg(*serial);
						receiveLog->push(std::pair<
								 timeStamp,
								 Msg>(
						    std::chrono::
							high_resolution_clock::
							    now(),
						    receiveMsg));
					} catch (std::exception &x) {
						std::cerr
						    << "SerialCommuincator "
						       "PreTransmit Reading: "
						    << x.what() << std::endl;
					}
				}

				MsgHandler handler;
				handler.retries = 10;
				handler.writeMsg(request.transmitMsg.value(),
				                 *serial);

				if (request.transmitPromise) {
					request.transmitPromise.value()
					    .set_value();
				}

				break;
			} catch (std::exception &x) {
				if (std::string(x.what()).compare("sWrite")
				    == 0) {
					continue;
				}
				if (request.transmitPromise) {
					request.transmitPromise.value()
					    .set_exception(
						std::make_exception_ptr(
						    std::runtime_error(
							x.what())));
				}
				if (request.receivePromise) {
					request.receivePromise.value()
					    .set_exception(
						std::make_exception_ptr(
						    std::runtime_error(
							x.what())));
				}
				return;
			}
		}
	}

	if (request.receivePromise) {
		while (true) {
			try {
				for (unsigned int tries = 100; tries; tries--) {
					if (serial->isDataAvailable())
						break;
					std::this_thread::sleep_for(
					    std::chrono::microseconds(500));
				}
				MsgHandler handler;
				serial->flushWrite();
				Msg receiveMsg = handler.readMsg(*serial);
				if (receiveMsg.type() == MsgType::DEBUG) {
					receiveLog->push(std::pair<timeStamp,
					                           Msg>(
					    std::chrono::high_resolution_clock::
						now(),
					    receiveMsg));
					continue;
				} else {
					request.receivePromise.value()
					    .set_value(receiveMsg);
					break;
				}
			} catch (std::exception &x) {
				request.receivePromise.value().set_exception(
				    std::make_exception_ptr(
					std::runtime_error(x.what())));
				break;
			}
		}
	}
}
} // namespace msg
