#ifndef _MSG_DUALPORT_HPP
#define _MSG_DUALPORT_HPP
#include <memory>
#include <msg/Stream.hpp>
#include <queue>
#include <vector>

namespace msg {
template <typename T> class DualPort {
public:
	class Port;

	DualPort()
	    : queueAtoB(), queueBtoA(),
	      A(queueBtoA, queueAtoB, mtxBtoA, mtxAtoB, conditionBtoA,
	        conditionAtoB),
	      B(queueAtoB, queueBtoA, mtxAtoB, mtxBtoA, conditionAtoB,
	        conditionBtoA) {}

	Port &GetPortA() { return this->A; }
	Port &GetPortB() { return this->B; }

private:
	std::queue<T> queueAtoB;
	std::queue<T> queueBtoA;
	std::mutex mtxAtoB;
	std::mutex mtxBtoA;
	std::condition_variable conditionAtoB;
	std::condition_variable conditionBtoA;
	Port A;
	Port B;
};

template <typename T> class DualPort<T>::Port : public msg::Serial<T> {
public:
	friend class DualPort;
	std::vector<T> read(unsigned int len);
	void write(const std::vector<T> data);

	void flush();
	void flushRead();
	void flushWrite();
	unsigned int numberOfBytesAvailable() const;
	bool isDataAvailable() const;
	bool isConnected() const;
	void setTimeouts(std::chrono::microseconds readTimeout,
	                 std::chrono::microseconds writeTimeout);

private:
	Port(std::queue<T> &readQueue, std::queue<T> &writeQueue,
	     std::mutex &readMtx, std::mutex &writeMtx,
	     std::condition_variable &readCondition,
	     std::condition_variable &writeCondition)
	    : readQueue(readQueue), writeQueue(writeQueue), readMtx(readMtx),
	      writeMtx(writeMtx), readCondition(readCondition),
	      writeCondition(writeCondition) {}
	std::queue<T> &readQueue;
	std::queue<T> &writeQueue;
	std::mutex &readMtx;
	std::mutex &writeMtx;
	std::condition_variable &readCondition;
	std::condition_variable &writeCondition;
	std::chrono::microseconds readTimeout = std::chrono::milliseconds(1000);
	std::chrono::microseconds writeTimeout =
	    std::chrono::milliseconds(1000);
};

template <typename T>
std::vector<T>
DualPort<T>::Port::read(unsigned int len) {
	namespace chrono = std::chrono;
	// Read Output
	std::vector<T> output;

	// Timeout Timer
	auto startTime = chrono::high_resolution_clock::now();
	auto elaspedTime = [startTime]() {
		return chrono::high_resolution_clock::now() - startTime;
	};

	do {
		std::unique_lock<std::mutex> readLock(this->readMtx);

		readCondition.wait_for(
		    readLock, std::chrono::microseconds(100),
		    [this] { return this->readQueue.size() > 0; });

		// If Data available
		while (this->readQueue.size() > 0 && output.size() < len) {
			// Read Data & delete from buffer
			output.push_back(this->readQueue.front());
			this->readQueue.pop();
		}
		// If finished reading, end
		if (output.size() == len) {
			break;
		}
	} while (elaspedTime() < this->readTimeout);

	// Check if sufficient data read
	if (output.size() != len) {
		throw std::runtime_error("Read Failed: Not all bytes read");
	}
	return output;
}

template <typename T>
void
DualPort<T>::Port::write(const std::vector<T> data) {
	// Multi-thread write saftey
	{
		auto lock = std::lock_guard(this->writeMtx);

		for (auto d : data) {
			this->writeQueue.push(d);
		}
	}
	this->writeCondition.notify_one();
}

template <typename T>
void
DualPort<T>::Port::flush() {
	this->flushRead();
	this->flushWrite();
}

template <typename T>
void
DualPort<T>::Port::flushRead() {
	// Multi-thread read saftey
	auto lock = std::lock_guard(this->readMtx);

	while (this->readQueue.size() > 0) {
		this->readQueue.pop(); // Potential data race with writer
	}
}

template <typename T>
void
DualPort<T>::Port::flushWrite() {
	// Multi-thread write saftey
	auto lock = std::lock_guard(this->writeMtx);

	while (this->writeQueue.size() > 0) {
		this->writeQueue.pop(); // Potential data race with reader
	}
}

template <typename T>
unsigned int
DualPort<T>::Port::numberOfBytesAvailable() const {
	return this->readQueue.size();
}

template <typename T>
bool
DualPort<T>::Port::isDataAvailable() const {
	return !this->readQueue.empty();
}

template <typename T>
bool
DualPort<T>::Port::isConnected() const {
	return true;
}

template <typename T>
void
DualPort<T>::Port::setTimeouts(std::chrono::microseconds readTimeout,
                               std::chrono::microseconds writeTimeout) {
	this->readTimeout = readTimeout;
	this->writeTimeout = writeTimeout;
}
} // namespace msg
#endif