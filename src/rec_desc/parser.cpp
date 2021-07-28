#include "parser.hpp"

#include "../exceptions/exceptions.hpp"
#include "runner.hpp"

std::string Parser::analyze(std::vector<Lexem> lexm) {
    query = lexm;
    return SQL_QUERY();
}

std::string Parser::SQL_QUERY() {
    lexem_iter = query.begin();
    end_iter = query.end();

    auto prxy = lexem_iter->get_token();

    if (lexem_iter == end_iter) {
        std::__throw_logic_error("unexpected EOS");
    } else {
        lexem_iter++;
    }

    switch (prxy) {
        case Lexem::SELECT:
            return SELECT();
        case Lexem::DROP:
            return DROP();
        case Lexem::CREATE:
            return CREATE();
        case Lexem::DELETE:
            return DELETE();
        case Lexem::INSERT:
            return INSERT();
        case Lexem::UPDATE:
            return UPDATE();
        default:
            return "Error: invalid input in analyze";
    }
}

void Parser::check_unexpected_EOF() {
    if (lexem_iter == end_iter) {
        throw std::logic_error("unexpected EOF");
    }
    lexem_iter++;
}

std::string Parser::SELECT() {
    // Select
    bool star = false;
    WhereCondition r;

    // Fields
    std::vector<std::string> field_names;
    field_names.clear();
    if (lexem_iter->get_token() == Lexem::MULT) {
        star = true;
        check_unexpected_EOF();
    } else {
        while (lexem_iter->get_token() == Lexem::IDENT ||
               lexem_iter->get_token() == Lexem::TEXT) {
            field_names.push_back(lexem_iter->get_string());
            check_unexpected_EOF();

            if (lexem_iter->get_token() == Lexem::COMMA) {
                check_unexpected_EOF();
            }
        }
    }

    // From
    if (lexem_iter->get_token() != Lexem::FROM) {
        throw std::logic_error("expected 'FROM' keyword");
    }

    // Table name
    check_unexpected_EOF();
    if (lexem_iter->get_token() != Lexem::TEXT &&
        lexem_iter->get_token() != Lexem::IDENT) {
        throw std::logic_error("expected table name");
    }
    std::string table_name = lexem_iter->get_string();

    // Where
    check_unexpected_EOF();
    if (lexem_iter->get_token() == Lexem::WHERE) {
        check_unexpected_EOF();
        r = WHERE();
    }

    std::string result;
    execute_select(field_names, table_name, r, result);

    return result;
}

std::string Parser::DROP() {
    if (lexem_iter->get_token() != Lexem::TABLE) {
        throw std::logic_error("expected 'TABLE' keyword");
    }

    check_unexpected_EOF();
    if (lexem_iter->get_token() != Lexem::TEXT &&
        lexem_iter->get_token() != Lexem::IDENT) {
        throw std::logic_error("expected table name");
    }
    std::string table_name = lexem_iter->get_string();

    // Calling drop function
    std::string result;
    execute_drop(table_name, result);
    return result;

    return "Error: invalid input in create";
}

std::string Parser::CREATE() {
    if (lexem_iter->get_token() != Lexem::TABLE) {
        throw std::logic_error("expected 'TABLE' keyword");
    }

    // Table name
    check_unexpected_EOF();
    std::string table_name;
    if (lexem_iter->get_token() != Lexem::TEXT &&
        lexem_iter->get_token() != Lexem::IDENT) {
        throw std::logic_error("expected table name");
    }
    table_name = lexem_iter->get_string();

    // Opening bracket
    check_unexpected_EOF();
    if (lexem_iter->get_token() != Lexem::OPEN) {
        throw std::logic_error("expected opening bracket");
    }

    // Table fields
    check_unexpected_EOF();
    std::vector<correlation> fields;
    fields.clear();

    while (lexem_iter->get_token() != Lexem::CLOSE) {
        correlation current_field;
        current_field.first = lexem_iter->get_string();

        check_unexpected_EOF();
        if (lexem_iter->get_token() == Lexem::TEXT ||
            lexem_iter->get_token() == Lexem::IDENT) {
            current_field.second = Text;
            check_unexpected_EOF();

            if (lexem_iter->get_token() != Lexem::OPEN) {
                throw std::logic_error("expected 'OPEN' keyword");
            }
            check_unexpected_EOF();

            if (lexem_iter->get_token() != Lexem::NUMBER) {
                throw std::logic_error("expected number");
            }
            // current_field.len = stoi(lexem_iter->get_string());

            check_unexpected_EOF();

            if (lexem_iter->get_token() != Lexem::CLOSE) {
                throw std::logic_error("expected 'CLOSE' keyword");
            }
            check_unexpected_EOF();
        } else if (lexem_iter->get_token() == Lexem::LONG) {
            current_field.second = Long;
            check_unexpected_EOF();
        } else {
            throw std::logic_error("expected either 'LONG' or 'TEXT' keyword");
        }

        if (lexem_iter->get_token() == Lexem::COMMA) {
            check_unexpected_EOF();
        }

        fields.push_back(current_field);
    }

    // Calling create function
    std::string result;
    execute_create(table_name, fields, result);
    return result;
}

std::string Parser::DELETE() {
    if (lexem_iter->get_token() != Lexem::FROM) {
        throw std::logic_error("expected 'FROM' keyword");
    }
    check_unexpected_EOF();

    if (lexem_iter->get_token() != Lexem::TEXT &&
        lexem_iter->get_token() != Lexem::IDENT) {
        throw std::logic_error("expected table name");
    }
    std::string table_name = lexem_iter->get_string();
    check_unexpected_EOF();

    if (lexem_iter->get_token() != Lexem::WHERE) {
        throw std::logic_error("expected 'WHERE' keyword");
    }

    check_unexpected_EOF();

    WhereCondition r = WHERE();

    // Calling delete function
    std::string result;
    execute_delete(table_name, r, result);
    return result;
}

std::string Parser::INSERT() {
    if (lexem_iter->get_token() != Lexem::INTO) {
        throw std::logic_error("expected 'INTO' keyword");
    }
    check_unexpected_EOF();

    if (lexem_iter->get_token() != Lexem::TEXT &&
        lexem_iter->get_token() != Lexem::IDENT) {
        throw std::logic_error("expected table name");
    }
    std::string table_name = lexem_iter->get_string();
    check_unexpected_EOF();

    if (lexem_iter->get_token() != Lexem::OPEN) {
        throw std::logic_error("expected 'OPEN' keyword");
    }
    check_unexpected_EOF();

    record new_rec;
    while (lexem_iter->get_token() != Lexem::CLOSE) {
        if (lexem_iter->get_token() == Lexem::STRING) {
            new_rec.push_back(std::make_shared<text_node>(lexem_iter->get_string().substr(
                1, lexem_iter->get_string().size() - 2)));
            check_unexpected_EOF();
        } else if (lexem_iter->get_token() == Lexem::NUMBER) {
            new_rec.push_back(std::make_shared<long_node>(std::stol(lexem_iter->get_string())));
            check_unexpected_EOF();
        } else if (lexem_iter->get_token() == Lexem::COMMA) {
            check_unexpected_EOF();
        } else {
            throw std::logic_error(
                "expected string or long typed fieldname or comma");
        }
    }

    // Calling insert function
    std::string result;
    execute_insert(table_name, new_rec, result);
    return result;
}

std::string Parser::UPDATE() {
    if (lexem_iter->get_token() != Lexem::TEXT &&
        lexem_iter->get_token() != Lexem::IDENT) {
        throw std::logic_error("expected table name");
    }
    std::string table_name = lexem_iter->get_string();
    check_unexpected_EOF();

    if (lexem_iter->get_token() != Lexem::SET) {
        throw std::logic_error("expected 'SET' keyword");
    }
    check_unexpected_EOF();

    std::vector<RPNExpr> expr;

    while (lexem_iter->get_token() != Lexem::WHERE) {
        if (lexem_iter->get_token() != Lexem::TEXT &&
            lexem_iter->get_token() != Lexem::IDENT) {
            throw std::logic_error("expected typename");
        }
        std::string field_name = lexem_iter->get_string();
        check_unexpected_EOF();

        if (lexem_iter->get_token() != Lexem::EQUAL) {
            throw std::logic_error("expected '='");
        }
        check_unexpected_EOF();

        std::vector<Lexem> lexems;
        while (lexem_iter->get_token() != Lexem::WHERE &&
               lexem_iter->get_token() != Lexem::COMMA) {
            Lexem::token t = lexem_iter->get_token();
            if (t == Lexem::TEXT || t == Lexem::IDENT) {
                Lexem::token lexems_end_token =
                    !lexems.empty() ? (lexem_iter - 1)->get_token()
                                    : Lexem::NOT;
                if (!lexems.empty() && (lexems_end_token == Lexem::IDENT ||
                                        lexems_end_token == Lexem::TEXT)) {
                    throw std::logic_error(
                        "two field names in a row are not allowed");
                }
                if (!lexems.empty() && lexems_end_token == Lexem::NUMBER) {
                    throw std::logic_error(
                        "number and a field name in a row is not allowed");
                }
                lexems.push_back(*lexem_iter);
            } else if (t == Lexem::DIV || t == Lexem::MULT ||
                       t == Lexem::PLUS || t == Lexem::MULT ||
                       t == Lexem::MULT || t == Lexem::MINUS ||
                       t == Lexem::PERCENT) {
                Lexem::token lexems_end_token =
                    !lexems.empty() ? (lexem_iter - 1)->get_token()
                                    : Lexem::NOT;
                if (!lexems.empty() && (lexems_end_token == Lexem::DIV ||
                                        lexems_end_token == Lexem::MULT ||
                                        lexems_end_token == Lexem::PLUS ||
                                        lexems_end_token == Lexem::MULT ||
                                        lexems_end_token == Lexem::MULT ||
                                        lexems_end_token == Lexem::MINUS ||
                                        lexems_end_token == Lexem::PERCENT)) {
                    throw std::logic_error(
                        "two operators in a row are not allowed");
                }
                lexems.push_back(*lexem_iter);
            } else if (t == Lexem::NUMBER) {
                Lexem::token lexems_end_token =
                    !lexems.empty() ? (lexem_iter - 1)->get_token()
                                    : Lexem::NOT;
                if (!lexems.empty() && lexems_end_token == Lexem::NUMBER) {
                    throw std::logic_error(
                        "two numbers in a row are not allowed");
                }
                if (!lexems.empty() && (lexems_end_token == Lexem::IDENT ||
                                        lexems_end_token == Lexem::TEXT)) {
                    throw std::logic_error(
                        "field name and a number in a row is not allowed");
                }
                lexems.push_back(*lexem_iter);
            }
            check_unexpected_EOF();
        }
        RPNExpr current;
        current.field_name = field_name;
        current.rpn = lexems;
        expr.push_back(current);
    }

    check_unexpected_EOF();

    WhereCondition r = WHERE();

    // Calling update function
    std::string result;
    execute_update(table_name, expr, r, result);
    return result;
}

WhereCondition Parser::WHERE() {
    struct WhereCondition result;

    // We need to know which alternative we have in the query. Firstly, we check
    // if it is the simplest one.
    if (lexem_iter->get_token() == Lexem::ALL) {
        result.type = Lexem::ALL;
        return result;
    }

    Lexem smth = *lexem_iter;
    check_unexpected_EOF();

    // At this point we know that 'smth' is not ALL lexem, but we still don't
    // know what it really is. It can be name of the field in LIKE-alternative,
    // can be a part of expression in IN alternative or in default WHERE clause.
    // If the next lexem is either NOT or LIKE, we have LIKE alternative. Let's
    // check it.
    if (lexem_iter->get_token() == Lexem::LIKE ||
        (lexem_iter != end_iter &&
         (lexem_iter->get_token() == Lexem::NOT &&
          (lexem_iter + 1)->get_token() == Lexem::LIKE))) {
        // Now we need to save the field name that 'smth' happens to be and then
        // get the expression string.
        result.type = Lexem::LIKE;
        result.l_lex_1 = smth;
        if (lexem_iter->get_token() == Lexem::NOT) {
            result.Not = true;

            // Now passing NOT and LIKE
            check_unexpected_EOF();
            check_unexpected_EOF();
        } else if (lexem_iter->get_token() == Lexem::NOT &&
                   lexem_iter == end_iter) {
            std::__throw_logic_error(
                "'NOT' keyword in the end of query is not allowed");
        } else {
            result.Not = false;
            // Going past LIKE keyword
            check_unexpected_EOF();
        }

        // Checking if we have a string.
        if (lexem_iter->get_token() == Lexem::STRING) {
            result.l_lex_2 = *lexem_iter;
        } else {
            std::__throw_logic_error("expected string");
        }

        return result;
    }

    // Now we don't know which alternative this is, but we need to get the
    // expression either way.
    std::vector<Lexem> lexems;
    lexems.push_back(smth);
    Lexem::token type = Lexem::WHERE;
    bool isNot = false;
    while (lexem_iter != end_iter) {
        Lexem::token t = lexem_iter->get_token();

        // Checking if we need to end the loop and which alternative we have.
        if (lexem_iter->get_token() == Lexem::IN ||
            (lexem_iter != end_iter &&
             (lexem_iter->get_token() == Lexem::NOT &&
              (lexem_iter + 1)->get_token() == Lexem::IN))) {
            type = Lexem::IN;
            break;
        } else if (lexem_iter->get_token() == Lexem::NOT &&
                   lexem_iter == end_iter) {
            std::__throw_logic_error(
                "'NOT' keyword in the end of query is not allowed");
        }

        if (t == Lexem::TEXT || t == Lexem::IDENT) {
            Lexem::token lexems_end_token =
                !lexems.empty() ? (lexem_iter - 1)->get_token() : Lexem::NOT;
            if (!lexems.empty() && (lexems_end_token == Lexem::IDENT ||
                                    lexems_end_token == Lexem::TEXT)) {
                throw std::logic_error(
                    "two field names in a row are not allowed");
            }
            if (!lexems.empty() && lexems_end_token == Lexem::NUMBER) {
                throw std::logic_error(
                    "number and a field name in a row is not allowed");
            }
            lexems.push_back(*lexem_iter);
        } else if (t == Lexem::SOFT_GREATER || t == Lexem::SOFT_LESSER ||
                   t == Lexem::NOT_EQUAL || t == Lexem::PERCENT ||
                   t == Lexem::GREATER || t == Lexem::LESSER ||
                   t == Lexem::MINUS || t == Lexem::CARET ||
                   t == Lexem::EQUAL || t == Lexem::MULT || t == Lexem::PLUS ||
                   t == Lexem::MULT || t == Lexem::MULT || t == Lexem::DIV ||
                   t == Lexem::NOT || t == Lexem::AND || t == Lexem::OR) {
            Lexem::token lexems_end_token =
                !lexems.empty() ? (lexem_iter - 1)->get_token() : Lexem::NOT;
            if (!lexems.empty() && (lexems_end_token == Lexem::SOFT_GREATER ||
                                    lexems_end_token == Lexem::SOFT_LESSER ||
                                    lexems_end_token == Lexem::NOT_EQUAL ||
                                    lexems_end_token == Lexem::PERCENT ||
                                    lexems_end_token == Lexem::GREATER ||
                                    lexems_end_token == Lexem::LESSER ||
                                    lexems_end_token == Lexem::MINUS ||
                                    lexems_end_token == Lexem::CARET ||
                                    lexems_end_token == Lexem::EQUAL ||
                                    lexems_end_token == Lexem::MULT ||
                                    lexems_end_token == Lexem::PLUS ||
                                    lexems_end_token == Lexem::MULT ||
                                    lexems_end_token == Lexem::MULT ||
                                    lexems_end_token == Lexem::DIV ||
                                    lexems_end_token == Lexem::NOT ||
                                    lexems_end_token == Lexem::AND ||
                                    lexems_end_token == Lexem::OR)) {
                throw std::logic_error(
                    "two operators in a row are not allowed");
            }
            lexems.push_back(*lexem_iter);
        } else if (t == Lexem::NUMBER) {
            Lexem::token lexems_end_token =
                !lexems.empty() ? (lexem_iter - 1)->get_token() : Lexem::NOT;
            if (!lexems.empty() && lexems_end_token == Lexem::NUMBER) {
                throw std::logic_error("two numbers in a row are not allowed");
            }
            if (!lexems.empty() && (lexems_end_token == Lexem::IDENT ||
                                    lexems_end_token == Lexem::TEXT)) {
                throw std::logic_error(
                    "field name and a number in a row is not allowed");
            }
            lexems.push_back(*lexem_iter);
        } else if (t == Lexem::OPEN || t == Lexem::CLOSE) {
            lexems.push_back(*lexem_iter);
        } else if (t == Lexem::STRING) {
            std::string s = lexem_iter->get_string();
            replace_all(s, "'", "");
            lexem_iter->set_string(s);
            lexem_iter->set_token(Lexem::STRING);
            lexems.push_back(*lexem_iter);
        }
        check_unexpected_EOF();
    }

    // Now we know what it is. Let's call the appropriate function.
    if (type == Lexem::WHERE) {
        // Simple WHERE clause, returning.
        result.type = Lexem::WHERE;
        result.v_lex_1 = lexems;
        return result;
    }
    // IN alternative.
    check_unexpected_EOF();

    if (lexem_iter->get_token() == Lexem::OPEN) {
        check_unexpected_EOF();

        std::vector<Lexem> consts;
        while (lexem_iter->get_token() != Lexem::CLOSE) {
            Lexem::token t = lexem_iter->get_token();
            if (t != Lexem::STRING && t != Lexem::LONG && t != Lexem::NUMBER) {
                std::__throw_logic_error("expected string or long constant");
            }
            consts.push_back(*lexem_iter);

            check_unexpected_EOF();

            while (lexem_iter->get_token() == Lexem::COMMA) {
                check_unexpected_EOF();
            }
        }

        // Returning.
        result.type = Lexem::IN;
        result.Not = isNot;
        result.v_lex_1 = lexems;
        result.v_lex_2 = consts;
    }

    return result;
}