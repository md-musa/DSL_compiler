#include <fstream>
#include <iostream>

#include "csv_utils.h"
#include "executor.h"
#include "parser.h"
#include "validator.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: ./logdsl query.dsl\n";
    return 1;
  }
  std::string queryFile = argv[1];
  Query q;
  if (!parseQueryFile(queryFile, q)) {
    std::cerr << "Failed to parse query\n";
    return 1;
  }

  //----  read header from CSV -------
  std::ifstream logFile(q.fileName);
  if (!logFile) {
    std::cerr << "Cannot open log file\n";
    return 1;
  }
  std::string headerLine;
  std::getline(logFile, headerLine);
  auto headers = parseCsvLine(headerLine);

  if (!validateQuery(q, headers)) return 1;

  executeQuery(q);
  return 0;
}
