#include <iostream>
#include <string>
#include <future>
#include <mutex>
#include <vector>
#include "boost/filesystem.hpp"
using namespace boost::filesystem;

class FileMonitor {
public:
  void push_back(std::string file) {
    std::lock_guard<std::mutex> lck(_mtx);
    _allFiles.push_back(std::move(file));
  }
  bool hasData() const {
    std::lock_guard<std::mutex> lck(_mtx);
    return !_allFiles.empty();
  }
  std::string pop_back() {
    std::lock_guard<std::mutex> lck(_mtx);
    std::string name = move(_allFiles.back());
    _allFiles.pop_back();
    return name;
  }

private:
  mutable std::mutex _mtx;
  std::vector<std::string> _allFiles;
};

void listDir(path pth, FileMonitor& fileSink) {
  for (directory_iterator it(pth); it != directory_iterator(); ++it) {
    if (is_regular_file(it->status())) {
      fileSink.push_back(it->path().filename().string());
    }
  }
}

void listDirs(std::vector<path> paths, FileMonitor& fileSink) {
  std::vector<std::future<void>> futures;
  for (auto& pth : paths) {
    std::cout << pth << std::endl;
    futures.emplace_back(std::async(listDir, pth, std::ref(fileSink)));
  }

  for (;;) {
    if (fileSink.hasData()) {
      std::string name = fileSink.pop_back();
      std::cout << name << std::endl;
    }
  }
  for (auto& fut : futures) {
    fut.wait();
  }
}

int main() {
  std::vector<path> paths;
  for (directory_iterator it(std::getenv("HOME")); it != directory_iterator();
       ++it) {
    if (is_directory(it->status())) {
      paths.push_back(it->path());
    }
  }
  FileMonitor fileSink;
  listDirs(paths, fileSink);
  return 0;
}
