#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

std::vector<std::thread> spawn() {
  std::vector<std::thread> threads;
  for (unsigned int i = 0; i < 16; ++i) {
    threads.emplace_back([&] {
      std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
      std::cout << "Hello from thread " << i << "!" << std::endl;
    });
  }
  return threads;
}

int main() {
  std::vector<std::thread> threads = spawn();
  std::cout << "Hello from main!\n";
  for (auto& t : threads) {
    t.join();
  }
}
