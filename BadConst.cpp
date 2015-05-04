#include <thread>
#include <future>
#include <iostream>

struct Counter {
  Counter(int k) : n(k) {}
  int n;
};

struct Holder {
  Holder(int n) : c(new Counter(n)) {}
  Counter* c;
};

int main() {
  const Holder h(10);
  std::future<void> fut = std::async(
      std::launch::deferred, [](const Holder& h) { ++(h.c->n); }, std::ref(h));
  std::cout << h.c->n << std::endl;
  fut.wait();
  std::cout << h.c->n << std::endl;
}
