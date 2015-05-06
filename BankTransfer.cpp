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

class Bank {
private:
  Account _accts[2];
  int _minBalance;

public:
  Bank(int minBalance) : _minBalance(minBalance) {
    _accts[0].deposit(minBalance);
  }
  void transfer(int from, int to, int sum) {
    if (_accts[from].balance() >= sum) {
      _accts[from].deposit(-sum);
      _accts[to].deposit(sum);
    }
  }
  void assertPositive() const {
    if (_accts[0].balance() < 0 || _accts[1].balance() < 0) {
      throw std::runtime_error("Negative balance!");
    }
  }
  void assertSolvent() const {
    if (_accts[0].balance() + _accts[1].balance() < _minBalance) {
      throw std::runtime_error("Need bailout!");
    }
  }
};

std::future<void> trans(Bank& bank, int from, int to, int sum) {
  return std::async(std::launch::async, [&](int from, int to, int sum) {
                                          bank.assertPositive();
                                          bank.transfer(from, to, sum);
                                          bank.assertSolvent();
                                        },
                    from, to, sum);
}

void test() {
  Bank bank(10);
  std::vector<std::future<void>> futures;
  for (int i = 0; i < 100000; ++i) {
    futures.emplace_back(trans(bank, 0, 1, 10));
    futures.emplace_back(trans(bank, 1, 0, 10));
  }
  for (auto& fut : futures) {
    fut.get();
  }
}

int main() {
  try {
    test();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
