#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <stack>
#include <stdexcept>

namespace _assembler {
  class InvalidNumber : public std::invalid_argument {
  public:
    InvalidNumber(std::string s) : std::invalid_argument("invalid number " + s) {};
  };
  class calculator {
    std::map<std::string, uint64_t>& labels;
    const std::string op_list = "+-*/!^&|><";
    int get_priority(std::string op) {
      if (op == "^") return 0;if (op == "&") return 0;if (op == "|") return 0;
      if (op == ">>") return 1;if (op == "<<") return 1;
      if (op == "<") return 2;if (op == ">") return 2;
      if (op == "<=") return 2;if (op == ">=") return 2;
      if (op == "==") return 2;if (op == "!=") return 2;
      if (op == "+") return 3;if (op == "-") return 3;
      if (op == "*") return 4;if (op == "/") return 4;if (op == "%") return 4;
      return 5;
    }
  int64_t to_int_with_base(const std::string& s, int base) {
    uint64_t ret = 0;
    for (char c : s) {
      if (c >= '0' && c <= '9') ret = ret * base + (c - '0');
      else if (c >= 'a' && c <= 'f') ret = ret * base + (c - 'a' + 10);
      else if (c >= 'A' && c <= 'F') ret = ret * base + (c - 'A' + 10);
      else throw InvalidNumber(s);
    }
    return *(int64_t*)&ret;
  }
  int64_t to_int(const std::string& s) {
    if (s[0] == '0' && s[1] == 'x') return to_int_with_base(s.substr(2), 16);
    if (s[0] == '0' && s[1] == 'b') return to_int_with_base(s.substr(2), 2);
    if (s[0] == '0' && s[1] == 'o') return to_int_with_base(s.substr(2), 8);
    return to_int_with_base(s, 10);
  }
  bool get_is_op(std::string op) { return std::string("+-*/!^&|><()?:").find(op[0]) != std::string::npos; }
  std::string get_next(std::string& line) {
    while (line[0] == ' ' || line[0] == '\t') if (line.size()) line = line.substr(1); else return "";
    if (line[0] == '\0') return "";
    std::string ret = line.substr(0, 1);line = line.substr(1);
    bool is_op = get_is_op(ret);
    if (is_op) if (op_list.find(ret[0]) == std::string::npos) return ret;
    while (line[0] != ' ' && line[0] != '\t') {
      if (line[0] == '\0') return ret;
      if (is_op != get_is_op(line.substr(0, 1))) return ret;
      ret += line[0];
      line = line.substr(1);
    }
    return ret;
  }
  uint64_t resolve(std::string line) {
    if (labels.find(line) != labels.end()) return labels[line];
    return to_int(line);
  }
  int64_t calculate(std::string op, int64_t a, int64_t b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") return a / b;
    if (op == "%") return a % b;
    if (op == ">>") return a >> b;
    if (op == "<<") return a << b;
    if (op == "^") return a ^ b;
    if (op == "&") return a & b;
    if (op == "|") return a | b;
    if (op == "!") return !a;
    if (op == "<") return a < b;
    if (op == ">") return a > b;
    if (op == "<=") return a <= b;
    if (op == ">=") return a >= b;
    if (op == "==") return a == b;
    if (op == "!=") return a != b;
    return 0;
  }
  int64_t calculate(std::string op, int64_t a) {
    if (op == "-") return -a;
    if (op == "!") return !a;
    if (op == "+") return a;
    return 0;
  }
  int64_t calc(std::string& line) {
      std::stack<std::string> _operator;
      std::stack<int64_t> _operand;
      while (line.size() > 0) {
        std::string op = get_next(line);
        if (op == "") break;
        if (op == ")") break;
        if (op == "(") {
          _operand.push(calc(line));
          continue;
        }
        if (op == "?") {
          auto cond = _operand.top();
          _operand.pop();
          auto if_true = calc(line);
          auto if_false = calc(line);
          _operand.push(cond ? if_true : if_false);
          continue;
        }
        if (op == ":") {
          break;
        }
        if (op_list.find(op[0]) != std::string::npos) {
          while (_operator.size() > 0) {
            std::string op2 = _operator.top();
            if (get_priority(op2) >= get_priority(op)) {
              int64_t b = _operand.top();
              _operand.pop();
              int64_t a = _operand.top();
              _operand.pop();
              _operand.push(calculate(op2, a, b));
              _operator.pop();
            } else break;
          }
          _operator.push(op);
        } else {
          _operand.push(resolve(op));
        }
      }
      while (_operator.size() > 0) {
        std::string op2 = _operator.top();
        _operator.pop();
        int64_t b = _operand.top();
        _operand.pop();
        if (_operand.size()) {
        int64_t a = _operand.top();
          _operand.pop();
          _operand.push(calculate(op2, a, b));
        } else {
          _operand.push(calculate(op2, b));
        }
      }
      return _operand.top();

  }
  public:
    calculator(std::map<std::string, uint64_t>& labels) : labels(labels) {}
    int64_t operator()(std::string line) {
      return calc(line);
    }
  };
}
