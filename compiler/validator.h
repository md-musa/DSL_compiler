#pragma once
#include <string>
#include <vector>

#include "ast.h"

bool validateQuery(Query& q, const std::vector<std::string>& header);
