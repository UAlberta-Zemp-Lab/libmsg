#include <chrono>

namespace msg {
typedef std::vector<uint8_t> byteArray;
typedef byteArray::const_iterator byteConstIter;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> timeStamp;
} // namespace msg