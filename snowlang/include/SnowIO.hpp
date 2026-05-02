#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sys/select.h>
#include <unistd.h>

namespace Snowlang {

struct SnowIO {
  virtual ~SnowIO() = default;

  virtual void write(const std::string &text) = 0;
  virtual void writeLn(const std::string &text) = 0;
  virtual bool hasLine() = 0;
  virtual std::string readLine() = 0;

  virtual bool writeFile(const std::string &path, const std::string &contents,
                         bool append = false) = 0;
  virtual bool readFile(const std::string &path, std::string &contents) = 0;
};

struct ConsoleIO : SnowIO {
  void write(const std::string &text) override { std::cout << text; }

  void writeLn(const std::string &text) override { std::cout << text << "\n"; }

  bool hasLine() override {
    fd_set set;
    timeval timeout{.tv_sec = 0, .tv_usec = 0};

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout) > 0;
  }

  std::string readLine() override {
    std::string s;
    std::getline(std::cin, s);
    return s;
  }

  bool writeFile(const std::string &path, const std::string &contents, bool append) override {
    std::ofstream file(path, append ? std::ios::app : std::ios::trunc);

    if (!file.is_open())
      return false;

    file << contents;
    return true;
  }

  bool readFile(const std::string &path, std::string &contents) override {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
      return false;

    contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    return true;
  }
};

struct BufferIO : SnowIO {
  std::vector<std::string> out;
  std::queue<std::string> in;
  std::unordered_map<std::string, std::string> files;

  void write(const std::string &text) override { out.push_back(text); }

  void writeLn(const std::string &text) override { out.push_back(text + "\n"); }

  bool hasLine() override { return !in.empty(); }

  std::string readLine() override {
    if (in.empty())
      return "";
    auto s = in.front();
    in.pop();
    return s;
  }

  bool writeFile(const std::string &path, const std::string &contents, bool append) override {
    if (append)
      files[path] += contents;
    else
      files[path] = contents;
    return true;
  }

  bool readFile(const std::string &path, std::string &contents) override {
    auto it = files.find(path);

    if (it == files.end())
      return false;

    contents = it->second;
    return true;
  }
};

} // namespace Snowlang