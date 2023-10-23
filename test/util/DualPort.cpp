#include "msg/stream/Stream.hpp"
#include <chrono>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

namespace msg {

template <class T> class DualPort {
public:
	template <class U> class Port;

	DualPort()
	    : queueAtoB(), queueBtoA(), A(queueBtoA, queueAtoB),
	      B(queueAtoB, queueBtoA) {}

	Port<T> &GetPortA() { return this->A; }
	Port<T> &GetPortB() { return this->B; }

private:
	std::queue<T> queueAtoB;
	std::queue<T> queueBtoA;
	Port<T> A;
	Port<T> B;
};

template <class T>
template <class U>
class DualPort<T>::Port : public msg::Serial<U> {
public:
	Port(std::queue<U> &readQueue, std::queue<U> &writeQueue)
	    : readQueue(readQueue), writeQueue(writeQueue) {}

	std::vector<U> read(unsigned int len) {
		namespace chrono = std::chrono;
		// Multi-thread read saftey
		auto lock = std::lock_guard(*(this->readMtx));

		// Read Output
		std::vector<U> output;

		// Timeout Timer
		auto startTime = chrono::high_resolution_clock::now();
		auto elaspedTime = [startTime]() {
			return chrono::high_resolution_clock::now() - startTime;
		};

		do {
			// If Data available
			while (this->readQueue.size() > 0
			       && output.size() < len) {
				// Read Data & delete from buffer
				output.push_back(this->readQueue.front());
				this->readQueue.pop();
			}
			// If finished reading, end
			if (output.size() == len) {
				break;
			}
			std::this_thread::sleep_for(
			    std::chrono::microseconds(100));
		} while (elaspedTime() < this->readTimeout);

		// Check if sufficient data read
		if (output.size() != len) {
			throw std::runtime_error(
			    std::format("Read Failed: {0}/{1} bytes read",
			                output.size(), len));
		}
		return output;
	}

	void write(const std::vector<U> data) {
		// Multi-thread write saftey
		auto lock = std::lock_guard(*(this->writeMtx));

		for (auto d : data) {
			this->writeQueue.push(d);
		}
	}

	void flush() {
		this->flushRead();
		this->flushWrite();
	}

	void flushRead() {
		// Multi-thread read saftey
		auto lock = std::lock_guard(*(this->readMtx));

		while (this->readQueue.size() > 0) {
			this->readQueue
			    .pop(); // Potential data race with writer
		}
	}

	void flushWrite() {
		// Multi-thread write saftey
		auto lock = std::lock_guard(*(this->writeMtx));

		while (this->writeQueue.size() > 0) {
			this->writeQueue
			    .pop(); // Potential data race with reader
		}
	}

	unsigned int numberOfBytesAvailable() const {
		return this->readQueue.size();
	}

	bool isDataAvailable() const { return !this->readQueue.empty(); }

	bool isConnected() const { return true; }

	void setTimeouts(std::chrono::microseconds readTimeout,
	                 std::chrono::microseconds writeTimeout) {
		this->readTimeout = readTimeout;
		this->writeTimeout = writeTimeout;
	}

private:
	std::queue<U> &readQueue;
	std::queue<U> &writeQueue;
	std::chrono::microseconds readTimeout = std::chrono::milliseconds(1000);
	std::chrono::microseconds writeTimeout =
	    std::chrono::milliseconds(1000);
	std::unique_ptr<std::mutex> readMtx = std::make_unique<std::mutex>();
	std::unique_ptr<std::mutex> writeMtx = std::make_unique<std::mutex>();
};

} // namespace msg