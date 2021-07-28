#ifndef SCANNER_H_
#define SCANNER_H_

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

class Lexem {
   public:
    enum token {
        OR,
        IN,
        NOT,
        ALL,
        AND,
        SET,
        DIV,
        END,
        TEXT,
        LONG,
        LIKE,
        MULT,
        PLUS,
        OPEN,
        INTO,
        FROM,
        DROP,
        WHERE,
        TABLE,
        MINUS,
        EQUAL,
        IDENT,
        COMMA,
        CLOSE,
        CARET,
        ERROR,
        SELECT,
        INSERT,
        UPDATE,
        DELETE,
        CREATE,
        LESSER,
        NUMBER,
        STRING,
        PERCENT,
        GREATER,
        NOT_EQUAL,
        UNDEFINED,
        UNDERSCORE,
        SOFT_LESSER,
        OPEN_SQUARE,
        SOFT_GREATER,
        CLOSE_SQUARE
    };

   private:
    enum token t;
    std::string s;

   public:
    Lexem();
    Lexem(std::string str);
    Lexem(const Lexem &other);
    Lexem &operator=(const Lexem &other) = default;
    enum token get_token();
    std::string get_string();
    void set_token(token tok);
    void set_string(std::string str);
};

void replace_all(std::string &str, const std::string &from,
                 const std::string &to);
std::vector<Lexem> parse(std::string str);

#endif