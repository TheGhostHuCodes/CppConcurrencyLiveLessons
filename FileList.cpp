#include <future>
#include <vector>
#include <string>
#include <iostream>
#include "boost/filesystem.hpp"
using namespace boost::filesystem;

std::vector<std::string> listDir(const path& dir) {
  std::vector<std::string> files;
  for (directory_iterator it(dir); it != directory_iterator(); ++it) {
    if (is_regular_file(it->status())) {
      files.push_back(it->path().filename().string());
    }
  }
  return files;
}

std::vector<std::string> listDirs(const std::vector<path>& paths) {
  std::vector<std::future<std::vector<std::string>>> futures;
  for (const auto& pth : paths) {
    std::cout << pth << std::endl;
    futures.emplace_back(std::async(listDir, std::ref(pth)));
  }
  std::vector<std::string> allFiles;
  for (auto& fut : futures) {
    auto files = fut.get();
    std::move(files.begin(), files.end(), std::back_inserter(allFiles));
  }
  return allFiles;
}

int main() {
  std::vector<path> paths;
  for (directory_iterator it(std::getenv("HOME")); it != directory_iterator();
       ++it) {
    if (is_directory(it->status())) {
      paths.push_back(it->path());
    }
  }
  for (auto name : listDirs(paths))
    std::cout << name << std::endl;
  return 0;
}
