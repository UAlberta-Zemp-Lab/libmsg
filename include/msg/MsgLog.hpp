#ifndef _MSG_LOG_HPP
#define _MSG_LOG_HPP

#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <msg/Msg.hpp>
#include <mutex>

namespace msg {
using timeStamp = std::chrono::time_point<std::chrono::high_resolution_clock>;
class MsgLog {
	std::deque<std::pair<timeStamp, Msg>> log;
	size_t logSize = 100;
	mutable std::mutex mtx;
	mutable std::condition_variable watchCondition;

public:
	MsgLog() noexcept;
	MsgLog(size_t) noexcept;
	MsgLog(const MsgLog &);
	MsgLog(MsgLog &&);
	MsgLog &operator=(MsgLog);
	~MsgLog();
	friend void swap(MsgLog &, MsgLog &) noexcept;

	decltype(log.size()) size();
	decltype(log.cbegin()) cbegin();
	decltype(log.cend()) cend();
	decltype(log) getLog();

	void push(Msg);
	void push(std::pair<timeStamp, Msg>);
	std::pair<timeStamp, Msg> pop();
	const std::pair<timeStamp, Msg> &peek() const;
	void watch(std::function<void(const decltype(log))> work,
	           std::function<std::function<bool()>(const decltype(log))>
	               workCondition) const;
	void watch_for(std::function<void(const decltype(log))> work,
	               const std::chrono::milliseconds timeout,
	               std::function<std::function<bool()>(const decltype(log))>
	                   workCondition) const;
};
} // namespace msg
#endif