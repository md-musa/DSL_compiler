#include "parser.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "csv_utils.h"

static std::string trim(const std::string& s) {
  auto start = s.find_first_not_of(" \t\r\n");
  auto end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  return s.substr(start, end - start + 1);
}

// parse a single condition with value possibly containing spaces and quotes
static Condition parseCondition(const std::string& condStr) {
  std::stringstream ss(condStr);
  std::string field, op, valRest;
  ss >> field >> op;
  std::getline(ss, valRest);
  valRest = trim(valRest);

  // remove quotes if present
  if (!valRest.empty() && valRest.front() == '"' && valRest.back() == '"') {
    valRest = valRest.substr(1, valRest.size() - 2);
  }

  Condition c;
  c.field = trim(field);
  if (op == "=")
    c.op = Op::EQ;
  else if (op == "!=")
    c.op = Op::NEQ;
  else if (op == "<")
    c.op = Op::LT;
  else if (op == "<=")
    c.op = Op::LE;
  else if (op == ">")
    c.op = Op::GT;
  else if (op == ">=")
    c.op = Op::GE;
  else if (op == "MATCH")
    c.op = Op::MATCH;

  c.value = valRest;
  return c;
}

// minimal parser, only supports SELECT fields, FROM file, optional WHERE with
// single condition, optional ORDER BY
bool parseQueryFile(const std::string& filename, Query& q) {
  std::ifstream in(filename);
  if (!in) return false;
  std::string line;
  std::string query;
  while (std::getline(in, line)) query += line + " ";
  query = trim(query);

  // Remove uppercasing to preserve field/value cases
  // std::transform(query.begin(), query.end(), query.begin(), ::toupper);

  size_t posSelect = query.find("SELECT ");
  size_t posFrom = query.find(" FROM ");
  if (posSelect == std::string::npos || posFrom == std::string::npos)
    return false;
  std::string selectPart =
      query.substr(posSelect + 7, posFrom - (posSelect + 7));
  selectPart = trim(selectPart);
  if (selectPart == "*")
    q.selectAll = true;
  else {
    std::stringstream ss(selectPart);
    std::string f;
    while (std::getline(ss, f, ',')) {
      q.selectFields.push_back(trim(f));
    }
  }

  size_t posWhere = query.find(" WHERE ");
  size_t posOrder = query.find(" ORDER BY ");
  q.fileName = trim(query.substr(
      posFrom + 6,
      (posWhere == std::string::npos ? posOrder : posWhere) - (posFrom + 6)));

  if (posWhere != std::string::npos) {
    q.hasWhere = true;
    size_t endWhere = (posOrder == std::string::npos ? query.size() : posOrder);
    std::string condStr = query.substr(posWhere + 7, endWhere - (posWhere + 7));
    // parse single condition (supports spaces in value)
    Condition c = parseCondition(condStr);
    q.whereExpr.conditions.push_back(c);
  }

  if (posOrder != std::string::npos) {
    q.hasOrderBy = true;
    std::string ordPart = query.substr(posOrder + 10);
    std::stringstream ss(ordPart);
    ss >> q.orderByField;
    std::string ordDir;
    ss >> ordDir;
    if (ordDir == "DESC") q.orderAsc = false;
  }

  if (q.selectFields.size() == 1 && q.selectFields[0] == "COUNT(*)")
    q.isCount = true;

  return true;
}
