#include <sstream>
#include <iomanip>
#include <string>
#include <chrono>

using namespace std::literals;
using Clock = std::chrono::system_clock;

std::string CurrentTimeToString()
{
    auto t = Clock::to_time_t(Clock::now());
    struct tm buf;
#ifdef _MSC_VER
    localtime_s(&buf, &t);
#else
    localtime_r(&t, &buf);
#endif
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << buf.tm_hour << ":" << buf.tm_min << ":" << buf.tm_sec;
    return std::move(os).str();
}