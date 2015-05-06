#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <mutex>

class Account {
public:
  Account(int balance = 0) : _balance(balance) {}
  Account(Account&) = delete;
  void deposit(int sum) {
    std::lock_guard<std::recursive_mutex> lck(_mutex);
    _balance += sum;
  }
  int balance() const {
    std::lock_guard<std::recursive_mutex> lck(_mutex);
    return _balance;
  }
  std::recursive_mutex& getMutex() { return _mutex; }

private:
  mutable std::recursive_mutex _mutex;
  int _balance;
};

class Bank {
public:
  Bank(int minBalance) : _minBalance(minBalance) {
    _accts[0].deposit(minBalance);
  }
  void transfer(int from, int to, int sum) {
    std::lock(_accts[from].getMutex(), _accts[to].getMutex());
    std::lock_guard<std::recursive_mutex> lck1(_accts[from].getMutex(),
                                               std::adopt_lock);
    std::lock_guard<std::recursive_mutex> lck2(_accts[to].getMutex(),
                                               std::adopt_lock);
    if (_accts[from].balance() >= sum) {
      _accts[from].deposit(-sum);
      _accts[to].deposit(sum);
    }
  }
  void assertPositive() {
    std::lock(_accts[0].getMutex(), _accts[1].getMutex());
    std::lock_guard<std::recursive_mutex> lck1(_accts[0].getMutex(),
                                               std::adopt_lock);
    std::lock_guard<std::recursive_mutex> lck2(_accts[1].getMutex(),
                                               std::adopt_lock);
    if (_accts[0].balance() < 0 || _accts[1].balance() < 0) {
      throw std::runtime_error("Negative balance!");
    }
  }
  void assertSolvent() {
    std::lock(_accts[0].getMutex(), _accts[1].getMutex());
    std::lock_guard<std::recursive_mutex> lck1(_accts[0].getMutex(),
                                               std::adopt_lock);
    std::lock_guard<std::recursive_mutex> lck2(_accts[1].getMutex(),
                                               std::adopt_lock);
    if (_accts[0].balance() + _accts[1].balance() < _minBalance) {
      throw std::runtime_error("Need bailout!");
    }
  }

private:
  Account _accts[2];
  int _minBalance;
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
  std::cout << "Banking simulation complete." << std::endl;
  return 0;
}
