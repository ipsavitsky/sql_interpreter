#ifndef RUNNER_H_
#define RUNNER_H_

#include <regex>
#include <string>
#include <vector>

#include "../dbms/table.hpp"
#include "parser.hpp"

enum RPNType { NUMERIC, LOGIC };

struct RPNExpr {
    std::string field_name;
    std::vector<Lexem> rpn;
};


enum UpdateExprType { FIELD_NAME, OPERATOR, NUMBER };

bool calculate(std::vector<Lexem> expr, std::string &result);
void trypop(std::stack<Lexem> opstack);
std::vector<Lexem> run_rpn(std::vector<Lexem> expr);
bool preprocess_lexm(table &table, record *cur_rec, Lexem &Lexem,
                     std::string &result_or_error);
bool preprocess(table &table, record *cur_rec, std::vector<Lexem> &rpn,
                std::string &result_or_error);
bool calculate_in(std::string value, std::vector<Lexem> values,
                  std::string &result_or_error);
bool calculate_like(std::string value, std::string condition,
                    std::string &result_or_error);
bool pipeline(table &table, record *currec, std::vector<Lexem> rpn,
              std::string &result_or_error);
bool where_pipeline(table &table, record *cur_rec, WhereCondition condition,
                    std::string &result_or_error);
bool get_table_field_names(table &table, std::vector<std::string> &field_names,
                           std::string &error);
bool execute_create(std::string table_name, std::vector<correlation> fields,
                    std::string &error);
bool execute_drop(std::string table_name, std::string &error);
bool execute_insert(std::string table_name,
                    record fields,
                    std::string &result_or_error);
bool execute_delete(std::string table_name, WhereCondition where_condition,
                    std::string &result_or_error);
bool execute_update(std::string table_name, std::vector<RPNExpr> rpns,
                    WhereCondition where_condition,
                    std::string &result_or_error);
bool execute_select(std::vector<std::string> field_names,
                    std::string table_name, WhereCondition where_condition,
                    std::string &result_or_error);

#endif