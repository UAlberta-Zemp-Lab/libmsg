#ifndef _MSG_DUALPORT_HPP
#define _MSG_DUALPORT_HPP
#include <condition_variable>
#include <iostream>
#include <memory>
#include <msg/Stream.hpp>
#include <queue>
#include <vector>

namespace msg {
template <typename T> class Port : public msg::Serial<T> {
public:
	static std::pair<Port<T>, Port<T>> GetPair();

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

	Port(const Port<T> &) = default;
	Port(Port<T> &&);
	Port<T> &operator=(Port<T>);
	~Port() = default;
	inline void static swap(Port<T> &first, Port<T> &second) {
		using std::swap;

		swap(first.readQueue, second.readQueue);
		swap(first.writeQueue, second.writeQueue);
		swap(first.readMutex, second.readMutex);
		swap(first.writeMutex, second.writeMutex);
		swap(first.readCondition, second.readCondition);
		swap(first.writeCondition, second.writeCondition);
		swap(first.readTimeout, second.readTimeout);
		swap(first.writeTimeout, second.writeTimeout);
	}

private:
	Port(std::shared_ptr<std::queue<T>> readQueue,
	     std::shared_ptr<std::queue<T>> writeQueue,
	     std::shared_ptr<std::mutex> readMtx,
	     std::shared_ptr<std::mutex> writeMtx,
	     std::shared_ptr<std::condition_variable> readCondition,
	     std::shared_ptr<std::condition_variable> writeCondition)
	    : readQueue(readQueue), writeQueue(writeQueue), readMtx(readMtx),
	      writeMtx(writeMtx), readCondition(readCondition),
	      writeCondition(writeCondition) {}
	std::shared_ptr<std::queue<T>> readQueue;
	std::shared_ptr<std::queue<T>> writeQueue;
	std::shared_ptr<std::mutex> readMtx;
	std::shared_ptr<std::mutex> writeMtx;
	std::shared_ptr<std::condition_variable> readCondition;
	std::shared_ptr<std::condition_variable> writeCondition;
	std::chrono::microseconds readTimeout = std::chrono::milliseconds(1000);
	std::chrono::microseconds writeTimeout =
	    std::chrono::milliseconds(1000);
};

template <typename T>
std::pair<Port<T>, Port<T>>
Port<T>::GetPair() {
	std::shared_ptr<std::queue<T>> queueAtoB =
	    std::make_shared<std::queue<T>>();
	std::shared_ptr<std::queue<T>> queueBtoA =
	    std::make_shared<std::queue<T>>();
	std::shared_ptr<std::mutex> mtxAtoB = std::make_shared<std::mutex>();
	std::shared_ptr<std::mutex> mtxBtoA = std::make_shared<std::mutex>();
	std::shared_ptr<std::condition_variable> conditionAtoB =
	    std::make_shared<std::condition_variable>();
	std::shared_ptr<std::condition_variable> conditionBtoA =
	    std::make_shared<std::condition_variable>();
	Port<T> A(queueBtoA, queueAtoB, mtxBtoA, mtxAtoB, conditionBtoA,
	          conditionAtoB);
	Port<T> B(queueAtoB, queueBtoA, mtxAtoB, mtxBtoA, conditionAtoB,
	          conditionBtoA);
	return { A, B };
}

template <typename T> Port<T>::Port(Port<T> &&other) {
	std::unique_lock<std::mutex> locks{ other.readMtx, other.writeMtx };
	std::lock_guard<std::mutex> readLock(other.readMtx, std::adopt_lock);
	std::lock_guard<std::mutex> writeLock(other.writeMtx, std::adopt_lock);

	swap(*this, other);
}
template <typename T>
Port<T> &
Port<T>::operator=(Port<T> other) {
	std::unique_lock<std::mutex> locks{ other.readMtx, other.writeMtx };
	std::lock_guard<std::mutex> readLock(other.readMtx, std::adopt_lock);
	std::lock_guard<std::mutex> writeLock(other.writeMtx, std::adopt_lock);

	swap(*this, other);
}

template <typename T>
std::vector<T>
Port<T>::read(unsigned int len) {
	namespace chrono = std::chrono;
	// Read Output
	std::vector<T> output;

	// Timeout Timer
	auto startTime = chrono::high_resolution_clock::now();
	auto elaspedTime = [startTime]() {
		return chrono::high_resolution_clock::now() - startTime;
	};

	do {
		std::unique_lock<std::mutex> readLock(*readMtx);

		readCondition->wait_for(
		    readLock, std::chrono::microseconds(1),
		    [this] { return readQueue->size() > 0; });

		// If Data available
		while (readQueue->size() > 0 && output.size() < len) {
			// Read Data & delete from buffer
			output.push_back(readQueue->front());
			readQueue->pop();
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
Port<T>::write(const std::vector<T> data) {
	// Multi-thread write saftey
	{
		auto lock = std::lock_guard(*writeMtx);

		for (auto d : data) {
			writeQueue->push(d);
		}
	}
	writeCondition->notify_one();
}

template <typename T>
void
Port<T>::flush() {
	this->flushRead();
	this->flushWrite();
}

template <typename T>
void
Port<T>::flushRead() {
	// Multi-thread read saftey
	auto lock = std::lock_guard(*readMtx);

	while (readQueue->size() > 0) {
		readQueue->pop(); // Potential data race with writer
	}
}

template <typename T>
void
Port<T>::flushWrite() {
	// Multi-thread write saftey
	auto lock = std::lock_guard(*writeMtx);

	while (writeQueue->size() > 0) {
		writeQueue->pop(); // Potential data race with reader
	}
}

template <typename T>
unsigned int
Port<T>::numberOfBytesAvailable() const {
	return static_cast<unsigned int>(readQueue->size());
}

template <typename T>
bool
Port<T>::isDataAvailable() const {
	return !readQueue->empty();
}

template <typename T>
bool
Port<T>::isConnected() const {
	return readQueue.use_count() > 1 && writeQueue.use_count() > 1;
}

template <typename T>
void
Port<T>::setTimeouts(std::chrono::microseconds _readTimeout,
                     std::chrono::microseconds _writeTimeout) {
	this->readTimeout = _readTimeout;
	this->writeTimeout = _writeTimeout;
}
} // namespace msg
#endif