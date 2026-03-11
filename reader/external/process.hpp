#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

namespace _external {
class Process {
  std::string name;
  std::vector<std::string> args;
  int pid;
  int base_fd[3];
  std::string output_buffer[2];
  std::string read_all(bool stderr = false) {
    std::string result;
    char buf[1024];
    while (true) {
      ssize_t n = ::read(base_fd[stderr ? 2 : 1], buf, sizeof(buf));
      if (n <= 0) {
        break;
      }
      result.append(buf, n);
    }
    return result;
  }
public:
  Process(const std::string &name, const std::vector<std::string> &args = {}) 
    : name(name), args(args) {
    int stdin_fd[2];
    int stdout_fd[2];
    int stderr_fd[2];
    ::pipe(stdin_fd);
    ::pipe(stdout_fd);
    ::pipe(stderr_fd);
    base_fd[0] = stdin_fd[0];
    base_fd[1] = stdout_fd[1];
    base_fd[2] = stderr_fd[1];
    if ((pid = ::fork()) == 0) {
      std::vector<char*> argv;
      std::transform(args.begin(), args.end(), std::back_inserter(argv), [](const std::string &s) { return const_cast<char*>(s.c_str()); });
      ::dup2(stdin_fd[1], 0);
      ::dup2(stdout_fd[0], 1);
      ::dup2(stderr_fd[0], 2);
      ::close(stdin_fd[1]);
      ::close(stdout_fd[0]);
      ::close(stderr_fd[0]);
      ::close(stdin_fd[0]);
      ::close(stdout_fd[1]);
      ::close(stderr_fd[1]);
      ::execvp(name.c_str(), argv.data());
      ::exit(1);
    }
    ::close(stdin_fd[1]);
    ::close(stdout_fd[0]);
    ::close(stderr_fd[0]);
  }
  bool wait(bool blocking = true) {
    int status;
    if (blocking) {
      ::waitpid(pid, &status, 0);
    } else {
      ::waitpid(pid, &status, WNOHANG);
    }
    return WIFEXITED(status);
  }
  void write(const std::string &s) {
    ::write(base_fd[0], s.c_str(), s.size());
  }
  std::string readline(bool stderr = false) {
    if (output_buffer[stderr].empty()) {
      output_buffer[stderr] = read_all(stderr);
    }
    auto pos = output_buffer[stderr].find('\n');
    if (pos == std::string::npos) {
      return "";
    }
    auto result = output_buffer[stderr].substr(0, pos + 1);
    output_buffer[stderr].erase(0, pos + 1);
    return result;
  }
  ~Process() {
    ::close(base_fd[0]);
    ::close(base_fd[1]);
    ::close(base_fd[2]);
  }
};
}
