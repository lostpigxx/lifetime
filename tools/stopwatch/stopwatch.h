// Author: LostPigxx
// E-mail: simplelx@zju.edu.cn
// 	  ٩(⊙o⊙*)و好!很有精神!
//
// StopWatch for c++

#include <chrono>

using namespace std::chrono;

class TimerBase {
public:
    TimerBase() : start_point_(steady_clock::time_point::min()) {}

    ~TimerBase() = default;

    void Start() {
        start_point_ = steady_clock::now();
    }

    void Clear() {
        start_point_ = steady_clock::time_point::min();
    }

    bool IsStarted() const {
        return (start_point_.time_since_epoch() != steady_clock::duration(0));
    }

    uint64_t GetSecond() {
        if (IsStarted()) {
            return duration_cast<seconds>(steady_clock::now() - start_point_).count();
        }
        return 0;
    }

    uint64_t GetMillionSecond() {
        if (IsStarted()) {
            return duration_cast<milliseconds>(steady_clock::now() - start_point_).count();
        }
        return 0;
    }

    uint64_t GetMicroSecond() {
        if (IsStarted()) {
            return duration_cast<microseconds>(steady_clock::now() - start_point_).count();
        }
        return 0;
    }

private:
    steady_clock::time_point start_point_;
};
