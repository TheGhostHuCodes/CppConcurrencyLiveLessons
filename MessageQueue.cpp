#include <iostream>
#include <string>
#include <deque>
#include <future>
#include <mutex>
#include <condition_variable>

template <typename T> class MessageQueue {
public:
  void send(T msg) {
    std::lock_guard<std::mutex> lck(_mtx);
    _messages.push_back(std::move(msg));
    _cond.notify_one();
  }
  T receive() {
    std::unique_lock<std::mutex> lck(_mtx);
    _cond.wait(lck, [this]() { return !_messages.empty(); });
    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
  }

private:
  mutable std::mutex _mtx;
  mutable std::condition_variable _cond;
  std::deque<T> _messages;
};

int main() {
  MessageQueue<int> queue;
  std::future<void> fut = std::async([&queue]() { queue.send(44); });
  int i = queue.receive();
  std::cout << "Received: " << i << std::endl;
  fut.wait();
  return 0;
}
