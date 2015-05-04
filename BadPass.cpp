#include <thread>
#include <future>
#include <memory>
#include <iostream>

struct Counter {
  Counter(int k) : n(k) {}
  int n;
};

int main() {
  Counter* pCount = new Counter(10);
  std::unique_ptr<Counter> puCount(pCount);
  std::future<void> fut = std::async(
      [](std::unique_ptr<Counter> p) { ++(p->n); }, std::move(puCount));
  std::cout << pCount->n << std::endl;
  fut.wait();
  return 0;
}
