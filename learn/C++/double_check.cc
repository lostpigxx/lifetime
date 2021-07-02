#include <array>
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

constexpr uint64_t BUFFER_SIZE = 4096;

using SharedCharArray = std::shared_ptr<std::array<char, BUFFER_SIZE>>;

class Buffer {
public:
    Buffer() = default;

    SharedCharArray GetBuffer() {
        return data_;
    }

    bool Init() {
        data_ = std::make_shared<std::array<char, BUFFER_SIZE>>();
    }

private:
    SharedCharArray data_;
};

void InitBuffer(Buffer& buf) {
    buf.Init();
}

int main() {
    Buffer buf;
    std::thread t0(InitBuffer, std::ref(buf));
    std::thread t1(InitBuffer, std::ref(buf));
}