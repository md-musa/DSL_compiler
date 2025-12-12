#include "executor.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>

#include "csv_utils.h"
#include "utils.h"

bool evaluateCondition(const Condition& c, const std::vector<std::string>& row,
                       const std::unordered_map<std::string, int>& hmap) {
  auto it = hmap.find(toLower(c.field));
  if (it == hmap.end()) return false;
  std::string val = row[it->second];
  if (c.op == Op::MATCH) {
    try {
      std::regex re(c.value);
      return std::regex_search(val, re);
    } catch (...) {
      return false;
    }
  }
  bool leftNum = !val.empty() &&
                 std::all_of(val.begin(), val.end(),
                             [](char ch) { return isdigit(ch) || ch == '-'; });
  bool rightNum = !c.value.empty() &&
                  std::all_of(c.value.begin(), c.value.end(),
                              [](char ch) { return isdigit(ch) || ch == '-'; });
  if (leftNum && rightNum) {
    long long L = std::stoll(val);
    long long R = std::stoll(c.value);
    switch (c.op) {
      case Op::EQ:
        return L == R;
      case Op::NEQ:
        return L != R;
      case Op::LT:
        return L < R;
      case Op::GT:
        return L > R;
      case Op::LE:
        return L <= R;
      case Op::GE:
        return L >= R;
      default:
        return false;
    }
  } else {
    switch (c.op) {
      case Op::EQ:
        return val == c.value;
      case Op::NEQ:
        return val != c.value;
      case Op::LT:
        return val < c.value;
      case Op::GT:
        return val > c.value;
      case Op::LE:
        return val <= c.value;
      case Op::GE:
        return val >= c.value;
      default:
        return false;
    }
  }
}

bool evaluateWhereExpr(const ConditionExpr& expr,
                       const std::vector<std::string>& row,
                       const std::unordered_map<std::string, int>& hmap) {
  if (expr.conditions.empty()) return true;
  bool res = evaluateCondition(expr.conditions[0], row, hmap);
  for (size_t i = 1; i < expr.conditions.size(); ++i) {
    bool next = evaluateCondition(expr.conditions[i], row, hmap);
    Logic conn = expr.connectors[i - 1];
    if (conn == Logic::AND)
      res = res && next;
    else if (conn == Logic::OR)
      res = res || next;
  }
  return res;
}

void executeQuery(const Query& q) {
  std::ifstream in(q.fileName);
  if (!in) {
    std::cerr << "Cannot open " << q.fileName << "\n";
    return;
  }

  std::string headerLine;
  std::getline(in, headerLine);
  auto headers = parseCsvLine(headerLine);
  std::unordered_map<std::string, int> hmap;
  for (size_t i = 0; i < headers.size(); ++i)
    hmap[toLower(headers[i])] = (int)i;

  std::vector<std::vector<std::string>> results;
  std::string line;
  long long count = 0;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    auto row = parseCsvLine(line);
    if (evaluateWhereExpr(q.whereExpr, row, hmap)) {
      if (q.isCount) {
        ++count;
      } else if (!q.hasOrderBy) {
        if (q.selectAll) {
          for (size_t i = 0; i < row.size(); ++i) {
            if (i) std::cout << ",";
            std::cout << row[i];
          }
          std::cout << "\n";
        } else {
          for (size_t i = 0; i < q.selectFields.size(); ++i)
            std::cout << (i ? "," : "")
                      << row[hmap[toLower(q.selectFields[i])]];
          std::cout << "\n";
        }
      } else
        results.push_back(std::move(row));
    }
  }

  if (q.isCount) {
    std::cout << "COUNT=" << count << "\n";
    return;
  }

  if (q.hasOrderBy) {
    int idx = hmap[toLower(q.orderByField)];
    std::sort(results.begin(), results.end(),
              [&](const auto& a, const auto& b) {
                if (q.orderAsc)
                  return a[idx] < b[idx];
                else
                  return a[idx] > b[idx];
              });
    for (auto& r : results) {
      if (q.selectAll) {
        for (size_t i = 0; i < r.size(); ++i) {
          if (i) std::cout << ",";
          std::cout << r[i];
        }
        std::cout << "\n";
      } else {
        for (size_t i = 0; i < q.selectFields.size(); ++i)
          std::cout << (i ? "," : "") << r[hmap[toLower(q.selectFields[i])]];
        std::cout << "\n";
      }
    }
  }
}
