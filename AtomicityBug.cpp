#include <iostream>
#include <list>
#include <thread>
#include <future>
#include <atomic>

class List {
  class Node {
  private:
    int _data;
    Node* _next;

  public:
    Node(int data, Node* next) : _data(data), _next(next) {}
    Node* getNext() const { return _next; }
  };
  std::atomic<Node*> _head;

public:
  List() : _head(nullptr) {}
  void push(int data) {
    Node* node = new Node(data, _head);
    _head = node;
  }
  bool empty() const { return _head.load() == nullptr; }
  void pop() {
    Node* top = _head.load();
    _head = _head.load()->getNext();
    delete top;
  }
};

int main() {
  List list;
  std::list<std::future<void>> futures;
  for (int i = 0; i < 100000; ++i) {
    futures.emplace_back(std::async(std::launch::async, [&list](int i) {
                                                          list.push(i);

                                                        },
                                    i));
  }
  while (!futures.empty()) {
    futures.back().get();
    futures.pop_back();
  }
  int len = 0;
  while (!list.empty()) {
    ++len;
    list.pop();
  }
  std::cout << len << std::endl;
  return 0;
}
