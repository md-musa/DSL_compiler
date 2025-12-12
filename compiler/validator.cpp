#include "validator.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "utils.h"

bool validateQuery(Query& q, const std::vector<std::string>& header) {
  std::unordered_map<std::string, int> headerMap;
  for (size_t i = 0; i < header.size(); ++i)
    headerMap[toLower(header[i])] = (int)i;

  // check SELECT fields
  if (!q.selectAll && !q.isCount) {
    for (auto& f : q.selectFields) {
      if (headerMap.find(toLower(f)) == headerMap.end()) {
        std::cerr << "Unknown field in SELECT: " << f << "\n";
        return false;
      }
    }
  }

  // check WHERE fields
  if (q.hasWhere) {
    for (auto& c : q.whereExpr.conditions) {
      if (headerMap.find(toLower(c.field)) == headerMap.end()) {
        std::cerr << "Unknown field in WHERE: " << c.field << "\n";
        return false;
      }
    }
  }

  // check ORDER BY
  if (q.hasOrderBy &&
      headerMap.find(toLower(q.orderByField)) == headerMap.end()) {
    std::cerr << "Unknown field in ORDER BY: " << q.orderByField << "\n";
    return false;
  }

  return true;
}
