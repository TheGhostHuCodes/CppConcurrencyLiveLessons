#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include "boost/filesystem.hpp"
using namespace boost::filesystem;

class FileMonitor {
public:
  void push_back(std::string file) {
    std::lock_guard<std::mutex> lck(_mtx);
    _allFiles.push_back(move(file));
  }

  void print() {
    std::lock_guard<std::mutex> lck(_mtx);
    for (auto& name : _allFiles) {
      std::cout << name << std::endl;
    }
  }

private:
  std::vector<std::string> _allFiles;
  std::mutex _mtx;
};

void listDir(path pth, FileMonitor& fileSink) {
  for (directory_iterator it(pth); it != directory_iterator(); ++it) {
    if (is_regular_file(it->status())) {
      fileSink.push_back(it->path().filename().string());
    }
  }
}

void listDirs(std::vector<path>& paths, FileMonitor& fileSink) {
  std::vector<std::future<void>> futures;
  for (auto& pth : paths) {
    std::cout << pth << std::endl;
    futures.emplace_back(std::async(listDir, pth.string(), std::ref(fileSink)));
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
  fileSink.print();
  return 0;
}
