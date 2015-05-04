#include <thread>
#include <iostream>
#include <string>

void repeat(std::string& str, int n) {
  if (n > 0) {
    str[0] = '*';
    std::cout << str << std::endl;
    repeat(str, n - 1);
  }
}

int main() {
  std::string str("blah");
  std::thread t(repeat, std::ref(str), 3);
  std::cout << "Hello from main!\n";
  t.join();
  std::cout << "After join: " << str << std::endl;
  return 0;
}
