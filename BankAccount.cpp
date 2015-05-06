#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <mutex>

class Account {
public:
  Account() : _balance(0) {}
  void deposit(int sum) {
    std::lock_guard<std::mutex> lck(_mutex);
    _balance += sum;
  }
  int balance() const {
    std::lock_guard<std::mutex> lck(_mutex);
    return _balance;
  }

private:
  mutable std::mutex _mutex;
  int _balance;
};

std::future<void> depositor(Account& acct, int sum) {
  return std::async([&](int sum) { acct.deposit(sum); }, sum);
}

std::future<void> balancer(const Account& acct) {
  return std::async([&]() { std::cout << acct.balance() << std::endl; });
}

void test() {
  Account acct;
  std::vector<std::future<void>> futures;
  futures.emplace_back(depositor(acct, 10));
  futures.emplace_back(depositor(acct, 20));
  futures.emplace_back(balancer(acct));
  futures.emplace_back(depositor(acct, -10));
  futures.emplace_back(depositor(acct, -10));
  futures.emplace_back(depositor(acct, -10));
  for (auto& fut : futures) {
    fut.wait();
  }
  std::cout << "Final balance " << acct.balance() << std::endl;
}

int main() {
  test();
  return 0;
}
