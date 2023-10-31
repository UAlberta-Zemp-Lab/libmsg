#ifndef _MSG_DUALPORT_H
#define _MSG_DUALPORT_H

namespace msg {
template <typename T> class DualPort {
public:
	class Port;

	DualPort()
	    : queueAtoB(), queueBtoA(), A(queueBtoA, queueAtoB),
	      B(queueAtoB, queueBtoA) {}

	Port &GetPortA() { return this->A; }
	Port &GetPortB() { return this->B; }

private:
	std::queue<T> queueAtoB;
	std::queue<T> queueBtoA;
	Port A;
	Port B;
};

template <typename T> class DualPort<T>::Port : public msg::Serial<T> {
public:
	Port(std::queue<T> &readQueue, std::queue<T> &writeQueue)
	    : readQueue(readQueue), writeQueue(writeQueue) {}

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
	std::queue<T> &readQueue;
	std::queue<T> &writeQueue;
	std::chrono::microseconds readTimeout = std::chrono::milliseconds(1000);
	std::chrono::microseconds writeTimeout =
	    std::chrono::milliseconds(1000);
	std::unique_ptr<std::mutex> readMtx = std::make_unique<std::mutex>();
	std::unique_ptr<std::mutex> writeMtx = std::make_unique<std::mutex>();
};

template <typename T>
std::vector<T>
DualPort<T>::Port::read(unsigned int len) {
	namespace chrono = std::chrono;
	// Multi-thread read saftey
	auto lock = std::lock_guard(*(this->readMtx));

	// Read Output
	std::vector<T> output;

	// Timeout Timer
	auto startTime = chrono::high_resolution_clock::now();
	auto elaspedTime = [startTime]() {
		return chrono::high_resolution_clock::now() - startTime;
	};

	do {
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
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	} while (elaspedTime() < this->readTimeout);

	// Check if sufficient data read
	if (output.size() != len) {
		throw std::runtime_error(std::format(
		    "Read Failed: {0}/{1} bytes read", output.size(), len));
	}
	return output;
}

template <typename T>
void
DualPort<T>::Port::write(const std::vector<T> data) {
	// Multi-thread write saftey
	auto lock = std::lock_guard(*(this->writeMtx));

	for (auto d : data) {
		this->writeQueue.push(d);
	}
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
	auto lock = std::lock_guard(*(this->readMtx));

	while (this->readQueue.size() > 0) {
		this->readQueue.pop(); // Potential data race with writer
	}
}

template <typename T>
void
DualPort<T>::Port::flushWrite() {
	// Multi-thread write saftey
	auto lock = std::lock_guard(*(this->writeMtx));

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