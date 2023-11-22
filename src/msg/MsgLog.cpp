#include <msg/MsgLog.hpp>
#include <mutex>

namespace msg {
using timeStamp = std::chrono::time_point<std::chrono::high_resolution_clock>;
MsgLog::MsgLog() noexcept {};
MsgLog::MsgLog(size_t logSize) noexcept : logSize(logSize){};
MsgLog::MsgLog(const MsgLog &other) : MsgLog(other.logSize) {
	std::lock_guard lock(other.mtx);
	log = other.log;
}
MsgLog::MsgLog(MsgLog &&other) : MsgLog(other.logSize) {
	std::lock_guard lock(other.mtx);
	swap(*this, other);
}
MsgLog &
MsgLog::operator=(MsgLog other) {
	std::lock_guard lock(other.mtx);
	swap(*this, other);
	return *this;
}
MsgLog::~MsgLog() { std::lock_guard lock(mtx); }
inline void
swap(MsgLog &first, MsgLog &second) noexcept {
	using std::swap;

	swap(first.log, second.log);
	swap(first.logSize, second.logSize);
}

decltype(MsgLog::log.size())
MsgLog::size() {
	return log.size();
}
decltype(MsgLog::log.cbegin())
MsgLog::cbegin() {
	return log.cbegin();
}
decltype(MsgLog::log.cend())
MsgLog::cend() {
	return log.cend();
}
decltype(MsgLog::log)
MsgLog::getLog() {
	return log;
}

void
MsgLog::push(Msg msg) {
	push(std::pair<timeStamp, Msg>(
	    std::chrono::high_resolution_clock::now(), msg));
}
void
MsgLog::push(std::pair<timeStamp, Msg> entry) {
	std::lock_guard lock(mtx);
	if (log.size() == logSize) {
		log.pop_front();
	}
	log.push_back(entry);
	watchCondition.notify_all();
}
std::pair<timeStamp, Msg>
MsgLog::pop() {
	std::lock_guard lock(mtx);
	std::pair<timeStamp, Msg> entry = log.front();
	log.pop_front();
	return entry;
}

const std::pair<timeStamp, Msg> &
MsgLog::peek() const {
	std::lock_guard lock(mtx);
	return log.front();
}

void
MsgLog::watch(std::function<void(const decltype(log))> work,
              std::function<std::function<bool()>(const decltype(log))>
                  workCondition) const {
	std::unique_lock lock(mtx);

	watchCondition.wait(lock, workCondition(log));

	work(log);
}

void
MsgLog::watch_for(std::function<void(const decltype(log))> work,
                  const std::chrono::milliseconds timeout,
                  std::function<std::function<bool()>(const decltype(log))>
                      workCondition) const {
	std::unique_lock lock(mtx);

	watchCondition.wait_for(lock, timeout, workCondition(log));

	work(log);
}
} // namespace msg