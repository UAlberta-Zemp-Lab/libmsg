/* See LICENSE file for copyright and license details. */
#ifndef _MSG_STREAM_HPP
#define _MSG_STREAM_HPP

#include <chrono>
#include <vector>

namespace msg {
template <class T> class Stream {
public:
	uint8_t flags = 0x0;
	virtual void write(const std::vector<T> data) = 0;
	virtual std::vector<T> read(unsigned int len) = 0;
	Stream<T> &operator<<(const std::vector<T> &data) {
		this->write(data);
		return *this;
	}
	Stream<T> &operator>>(std::vector<T> &data) {
		std::vector<T> readData = this->read(data.size());
		data.assign(readData.begin(), readData.end());
		return *this;
	}
};

template <class T> class Serial : public Stream<T> {
public:
	virtual void flush() = 0;
	virtual void flushRead() = 0;
	virtual void flushWrite() = 0;
	virtual unsigned int numberOfBytesAvailable() const = 0;
	virtual bool isDataAvailable() const = 0;
	virtual bool isConnected() const = 0;
	virtual void setTimeouts(std::chrono::microseconds readTimeout,
	                         std::chrono::microseconds writeTimeout) = 0;
};
} // namespace msg

#endif