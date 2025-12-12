#pragma once
#include "ast.h"
#include <string>

bool parseQueryFile(const std::string &filename, Query &q);
