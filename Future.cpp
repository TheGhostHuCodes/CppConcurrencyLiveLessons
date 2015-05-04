#include <thread>
#include <future>
#include <iostream>
#include <string>
#include <memory>

void fun(std::promise<std::string>&& pr) {
  std::cout << "Worker throwing.\n";
  try {
    throw std::runtime_error("Exception from thread.");
    pr.set_value("Message from thread.");
  } catch (...) {
    pr.set_exception(std::current_exception());
  }
}

int main() {
  std::promise<std::string> pr;
  std::future<std::string> fut = pr.get_future();
  std::thread th(fun, std::move(pr));
  std::cout << "Main receiving:\n";
  try {
    std::string str = fut.get();
    std::cout << str << std::endl;
  } catch (std::runtime_error& e) {
    std::cout << "Caught: " << e.what() << std::endl;
  }
  th.join();
  return 0;
}
