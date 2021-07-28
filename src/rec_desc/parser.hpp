#ifndef PARSER_H_
#define PARSER_H_

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <stack>

#include "scanner.hpp"

struct WhereCondition {
    Lexem::token type;
    std::vector<Lexem> v_lex_1;
    std::vector<Lexem> v_lex_2;
    Lexem l_lex_1;
    Lexem l_lex_2;
    bool Not;
};

class Parser {
   private:
    std::vector<Lexem> query;

    std::vector<Lexem>::iterator lexem_iter;
    std::vector<Lexem>::iterator end_iter;
    void check_unexpected_EOF();
    std::string SQL_QUERY();
    std::string SELECT();
    std::string DROP();
    std::string CREATE();
    std::string DELETE();
    std::string INSERT();
    std::string UPDATE();
    WhereCondition WHERE();

   public:
    std::string analyze(std::vector<Lexem> lexm);
};

#endif