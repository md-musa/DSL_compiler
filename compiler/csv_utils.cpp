#include "csv_utils.h"

#include <string>
#include <vector>

std::vector<std::string> parseCsvLine(const std::string& line) {
  std::vector<std::string> out;
  std::string cur;
  bool inQuote = false;
  for (size_t i = 0; i < line.size(); ++i) {
    char c = line[i];
    if (c == '"') {
      if (inQuote && i + 1 < line.size() && line[i + 1] == '"') {
        cur.push_back('"');
        ++i;
      } else {
        inQuote = !inQuote;
      }
    } else if (c == ',' && !inQuote) {
      out.push_back(cur);
      cur.clear();
    } else {
      cur.push_back(c);
    }
  }
  out.push_back(cur);
  return out;
}
