#pragma once
#include <string>
#include <vector>

enum class Op { EQ, NEQ, LT, GT, LE, GE, MATCH };
enum class Logic { NONE, AND, OR };

struct Condition {
    std::string field;
    Op op;
    std::string value;
};

struct ConditionExpr {
    std::vector<Condition> conditions;
    std::vector<Logic> connectors; // connectors[i] connects conditions[i] to conditions[i+1]
};

struct Query {
    bool selectAll = false;
    std::vector<std::string> selectFields;
    std::string fileName;
    bool hasWhere = false;
    ConditionExpr whereExpr;
    bool hasOrderBy = false;
    std::string orderByField;
    bool orderAsc = true;
    bool isCount = false;
};
