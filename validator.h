#pragma once
#include "ast.h"
#include <string>
#include <vector>

bool validateQuery(Query &q, const std::vector<std::string> &header);
