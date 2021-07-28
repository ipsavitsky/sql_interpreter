#include "runner.hpp"

bool calculate(std::vector<Lexem> expr, std::string &result) {
    std::stack<std::string> opndStack;
    for (auto i : expr) {
        int res;
        std::string s_res;
        std::string bodge;
        switch (i.get_token()) {
            case Lexem::TEXT:
            case Lexem::IDENT:
                bodge = i.get_string();
                bodge.resize(bodge.size() - 1);
                i.set_string(bodge);
            case Lexem::STRING:
                opndStack.push(i.get_string());
                break;
            case Lexem::NUMBER:
                opndStack.push(i.get_string());
                break;
            case Lexem::PLUS:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res += std::stoi(opndStack.top());
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::MINUS:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res = std::stoi(opndStack.top()) - res;
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::MULT:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res *= std::stoi(opndStack.top());
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::DIV:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res = std::stoi(opndStack.top()) / res;
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::PERCENT:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res = std::stoi(opndStack.top()) % res;
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::AND:
            case Lexem::OR:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res = std::stoi(opndStack.top()) && res;
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::CARET:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res = std::stoi(opndStack.top()) ^ res;
                opndStack.pop();
                opndStack.push(std::to_string(res));
                break;
            case Lexem::NOT_EQUAL:
                s_res = opndStack.top();
                opndStack.pop();
                s_res = (opndStack.top() != s_res) ? "1" : "0";
                opndStack.pop();
                opndStack.push(s_res);
                break;
            case Lexem::EQUAL:
                s_res = opndStack.top();
                opndStack.pop();
                s_res = (opndStack.top() == s_res) ? "1" : "0";
                opndStack.pop();
                opndStack.push(s_res);
                break;
            case Lexem::SOFT_GREATER:
                s_res = opndStack.top();
                opndStack.pop();
                s_res = opndStack.top() >= s_res ? "1" : "0";
                opndStack.pop();
                opndStack.push(s_res);
                break;
            case Lexem::SOFT_LESSER:
                s_res = opndStack.top();
                opndStack.pop();
                s_res = opndStack.top() <= s_res ? "1" : "0";
                opndStack.pop();
                opndStack.push(s_res);
                break;
            case Lexem::GREATER:
                s_res = opndStack.top();
                opndStack.pop();
                s_res = opndStack.top() > s_res ? "1" : "0";
                opndStack.pop();
                opndStack.push(s_res);
                break;
            case Lexem::LESSER:
                s_res = opndStack.top();
                opndStack.pop();
                s_res = opndStack.top() < s_res ? "1" : "0";
                opndStack.pop();
                opndStack.push(s_res);
                break;
            case Lexem::NOT:
                res = std::stoi(opndStack.top());
                opndStack.pop();
                res = !res;
                opndStack.push(std::to_string(res));
                break;
            default:
                result = "Unknown operator";
                return false;
        }
    }

    result = opndStack.top();
    return true;
}

void trypop(std::stack<Lexem> opstack) {
    if (opstack.empty()) {
        std::__throw_logic_error("stack underflow, caret not closed");
    }
}

std::vector<Lexem> run_rpn(std::vector<Lexem> expr) {
    std::vector<Lexem> out;
    std::stack<Lexem> opstack;

    for (auto i : expr) {
        Lexem::token t = i.get_token();
        switch (t) {
            case Lexem::NUMBER:
            case Lexem::IDENT:
            case Lexem::TEXT:
            case Lexem::STRING:
                out.push_back(i);
                break;
            case Lexem::MINUS:
            case Lexem::PLUS:
                while (!opstack.empty() &&
                       opstack.top().get_token() != Lexem::OPEN) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                opstack.push(i);
                break;
            case Lexem::MULT:
                while (!opstack.empty() &&
                       opstack.top().get_token() == Lexem::MULT) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                opstack.push(i);
                break;
            case Lexem::CARET:
            case Lexem::AND:
            case Lexem::OR:
                while ((!opstack.empty() &&
                        opstack.top().get_token() == Lexem::AND) ||
                       (!opstack.empty() &&
                        opstack.top().get_token() == Lexem::OR)) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                opstack.push(i);
                break;
            case Lexem::NOT_EQUAL:
            case Lexem::EQUAL:
                while ((!opstack.empty() &&
                        opstack.top().get_token() == Lexem::NOT_EQUAL) ||
                       (!opstack.empty() &&
                        opstack.top().get_token() == Lexem::EQUAL)) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                opstack.push(i);
                break;
            case Lexem::SOFT_GREATER:
            case Lexem::SOFT_LESSER:
            case Lexem::GREATER:
            case Lexem::LESSER:
                while ((!opstack.empty() &&
                        opstack.top().get_token() == Lexem::MULT) ||
                       (!opstack.empty() &&
                        opstack.top().get_token() == Lexem::SOFT_GREATER) ||
                       (!opstack.empty() &&
                        opstack.top().get_token() == Lexem::SOFT_LESSER) ||
                       (!opstack.empty() &&
                        opstack.top().get_token() == Lexem::GREATER) ||
                       (!opstack.empty() &&
                        opstack.top().get_token() == Lexem::LESSER)) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                opstack.push(i);
                break;
            case Lexem::NOT:
                while (!opstack.empty() &&
                       opstack.top().get_token() != Lexem::OPEN) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                opstack.push(i);
                break;
            case Lexem::CLOSE:
                while (opstack.top().get_token() != Lexem::OPEN) {
                    out.push_back(opstack.top());
                    trypop(opstack);
                    opstack.pop();
                }
                trypop(opstack);
                opstack.pop();
                break;
            case Lexem::OPEN:
                opstack.push(i);
                break;
            default:
                break;
        }
    }

    while (!opstack.empty()) {
        out.push_back(opstack.top());
        opstack.pop();
    }
    return out;
}

bool preprocess_lexm(table &table, record *cur_rec, Lexem &Lexem,
                     std::string &result_or_error) {
    // Nothing to preprocess if this is not a field name
    if ((Lexem::IDENT != Lexem.get_token()) &&
        (Lexem::TEXT != Lexem.get_token())) {
        return true;
    }

    // Getting field type
    fld_types typeField = table.get_field_type(Lexem.get_string());
    auto cors = table.get_correlations();
    std::size_t place = 0;
    for (const auto &k : cors) {
        if (k.first == Lexem.get_string()) {
            break;
        }
        place++;
    }
    // --place;
    // Replacing field name with the field value withing the Lexem
    std::string szValue;
    std::int64_t lValue = 0;
    switch (typeField) {
        case Text:
            szValue = (*cur_rec)[place].get()->get_text();
            Lexem.set_string(szValue);
            break;
        case Long:
            lValue = (*cur_rec)[place].get()->get_long();
            Lexem.set_string(std::to_string(lValue));
            break;
        default:
            break;
    }
    // Successfully processed if reached here
    return true;
}

bool preprocess(table &table, record *cur_rec, std::vector<Lexem> &rpn,
                std::string &result_or_error) {
    for (auto i = rpn.begin(); i != rpn.end(); i++) {
        if (!preprocess_lexm(table, cur_rec, *i, result_or_error)) {
            return false;
        }
    }
    return true;
}

bool calculate_in(std::string value, std::vector<Lexem> values,
                  std::string &result_or_error) {
    // Checking if "value" exists in the "values" list
    for (auto i : values) {
        switch (i.get_token()) {
            case Lexem::STRING:
            case Lexem::NUMBER:
                if (value == i.get_string()) {
                    result_or_error = "1";
                    return true;
                } else {
                    break;
                }
            default:
                result_or_error =
                    "Bad value type in the list of possible values in the IN "
                    "clause";
                return false;
        }
    }

    // Nothing found if reached here
    result_or_error = "0";
    return true;
}

bool calculate_like(std::string value, std::string condition,
                    std::string &result_or_error) {
    // std::size_t occurance = 0;

    replace_all(condition, "'", "");
    replace_all(condition, "\\", "\\\\");
    replace_all(condition, "$", "\\$");
    replace_all(condition, ".", "\\.");
    replace_all(condition, "*", "\\*");
    replace_all(condition, "+", "\\+");
    replace_all(condition, "?", "\\?");
    replace_all(condition, "{", "\\{");
    replace_all(condition, "}", "\\}");
    replace_all(condition, "|", "\\|");
    replace_all(condition, "(", "\\(");
    replace_all(condition, ")", "\\)");
    replace_all(condition, "%", ".*");
    replace_all(condition, "_", ".");


    result_or_error =
        std::to_string(std::regex_match(value, std::regex(condition)));

    return true;
}

bool pipeline(table &table, record *currec, std::vector<Lexem> rpn,
              std::string &result_or_error) {
    rpn = run_rpn(rpn);
    std::string strError;
    if (!preprocess(table, currec, rpn, strError)) {
        result_or_error = strError;
        return false;
    }

    // Calculating the resulting RPN
    return calculate(rpn, result_or_error);
}

bool where_pipeline(table &table, record *cur_rec, WhereCondition condition,
                    std::string &result_or_error) {
    // Processing trivial case
    if (Lexem::ALL == condition.type) {
        result_or_error = "1";
        return true;
    }

    // Rearranging Lexemems according to the operations priorities
    if ((Lexem::WHERE == condition.type) || (Lexem::IN == condition.type)) {
        condition.v_lex_1 = run_rpn(condition.v_lex_1);
    }

    // Replacing table field names with the corresponding table values in the
    // 1rst Lexemem vector
    switch (condition.type) {
        case Lexem::WHERE:
        case Lexem::IN:
            if (!preprocess(table, cur_rec, condition.v_lex_1,
                            result_or_error)) {
                return false;
            }
            break;
        case Lexem::LIKE:
            if (!preprocess_lexm(table, cur_rec, condition.l_lex_1,
                                 result_or_error)) {
                return false;
            }
            break;
        case Lexem::ALL:
            break;
        default:
            result_or_error = "Bad WhereResult.type value";
            return false;
    }

    // Calculating condition value
    switch (condition.type) {
        case Lexem::WHERE:
            if (!calculate(condition.v_lex_1, result_or_error)) {
                return false;
            }
            break;
        case Lexem::IN:
            if (!calculate(condition.v_lex_1, result_or_error)) {
                return false;
            }
            if (!calculate_in(result_or_error, condition.v_lex_2,
                              result_or_error)) {
                return false;
            }
            break;
        case Lexem::LIKE:
            if (!calculate_like(condition.l_lex_1.get_string(),
                                condition.l_lex_2.get_string(),
                                result_or_error)) {
                return false;
            }
            break;
        case Lexem::ALL:
            result_or_error = "1";
            break;
        default:
            result_or_error = "Bad WhereResult.type value";
            return false;
    }

    // Applying the NOT condition if necessary
    if (condition.Not) {
        switch (condition.type) {
            case Lexem::IN:
            case Lexem::LIKE:
                if ("0" == result_or_error) {
                    result_or_error = "1";
                } else if ("1" == result_or_error) {
                    result_or_error = "0";
                } else {
                    return false;
                }
                break;
            default:
                throw "kill me";
        }
    }

    // Everything is OK if reached here
    return true;
}

bool get_table_field_names(table &table, std::vector<std::string> &field_names,
                           std::string &error) {
    // Resetting field names array
    field_names.clear();

    auto cors = table.get_correlations();
    // field_names.resize(cors.size());
    for (const auto &k : cors) {
        field_names.emplace_back(k.first);
    }
    return true;
}

bool execute_create(std::string table_name, std::vector<correlation> fields,
                    std::string &error) {
    // Checking if we have table structure
    if (0 == fields.size()) {
        std::stringstream err;
        err << "Failed to CREATE table " << table_name.data()
            << ": No table fields specified";
        error.clear();
        error = err.str();
        return false;
    }

    table created_table(table_name);
    try {
        created_table.set_correlations(fields);
        created_table.write_header();
    } catch (std::exception &e) {
        std::stringstream err;
        err << "Failed to CREATE TABLE " << table_name.data() << ": "
            << e.what();
        error.clear();
        error = err.str();
        return false;
    }
    std::stringstream res;
    res << "Created table " << table_name.data();
    error.clear();
    error = res.str();
    return true;
}

bool execute_drop(std::string table_name, std::string &error) {
    table cur_table(table_name);
    try {
        cur_table.drop();
    } catch (std::exception &e) {
        std::stringstream roerr;
        roerr << "Failed to DROP the " << table_name.data()
              << " table file: " << e.what() << std::endl;
        error.clear();
        error = roerr.str();
        return false;
    }
    std::stringstream res;
    res << "Dropped table " << table_name.data();
    error.clear();
    error = res.str();
    return true;
}

bool execute_insert(std::string table_name, record fields,
                    std::string &result_or_error) {
    // Opening the table created
    table cur_table(table_name);
    cur_table.read_header();
    // TODO(me): make this the input
    try {
        cur_table.add_record(fields);
    } catch (std::exception &e) {
        std::stringstream roerr;
        roerr << "Failed to create new record: " << e.what() << std::endl;
        result_or_error.clear();
        result_or_error = roerr.str();
        return false;
    }
    return true;
}

bool execute_delete(std::string table_name, WhereCondition where_condition,
                    std::string &result_or_error) {
    table cur_table(table_name);
    cur_table.read_header();
    for (auto cur = cur_table.begin(); cur != cur_table.end(); ++cur) {
        std::string where_result;
        auto prxy = *cur;
        if (!where_pipeline(cur_table, &prxy, where_condition, where_result)) {
            return false;
        } else if ("1" != where_result) {
            continue;
        }

        // Deleting the record
        try {
            cur_table.delete_record(cur.get_place());
        } catch (std::exception &e) {
            std::stringstream roerr;
            roerr << "Failed to delete record: " << e.what() << std::endl;
            result_or_error.clear();
            result_or_error = roerr.str();
            return false;
        }
    }
    // Closing the table and exiting
    std::stringstream res;
    res << "Deleted from table " << table_name.data();
    result_or_error.clear();
    result_or_error = res.str();
    return true;
}

bool execute_update(std::string table_name, std::vector<RPNExpr> rpns,
                    WhereCondition where_condition,
                    std::string &result_or_error) {
    // Opening the table created
    table cur_table(table_name);
    cur_table.read_header();
    // auto cursor = cur_table.begin();

    // Looping through records
    for (auto cursor = cur_table.begin(); cursor != cur_table.end(); ++cursor) {
        // Checking if the current record meets the "where" condition
        std::string where_result;
        auto prxy = *cursor;
        if (!where_pipeline(cur_table, &prxy, where_condition, where_result)) {
            return false;
        } else if ("1" != where_result) {
            continue;
        }

        auto currec = *cursor;

        // Looping through the fields array
        for (auto i = rpns.begin(); i != rpns.end(); i++) {
            // Calculating the RPN to put result to the database table
            if (!pipeline(cur_table, &currec, i->rpn, result_or_error)) {
                std::stringstream roerr;
                roerr << "Failed to pipeline an RPN for the: " << i->field_name
                      << "field of the " << table_name
                      << " table: " << result_or_error;
                result_or_error.clear();
                result_or_error = roerr.str();
                return false;
            }

            // Getting corresponding field type
            fld_types typeField;
            try {
                typeField = cur_table.get_field_type(i->field_name);
            } catch (std::exception &e) {
                std::stringstream roerr;
                roerr << "Failed to read the " << i->field_name.data()
                      << " field type from the " << table_name.data()
                      << " table: " << e.what() << std::endl;
                result_or_error.clear();
                result_or_error = roerr.str();
                return false;
            }
            // Sending field data to the database
            auto cors = cur_table.get_correlations();
            std::size_t place = 0;
            for (auto cur : cors) {
                if (cur.first == i->field_name) {
                    break;
                }
                place++;
            }
            switch (typeField) {
                case Text:
                    currec[place] =
                        std::make_shared<text_node>(result_or_error);
                    break;
                case Long:
                    currec[place] =
                        std::make_shared<long_node>(std::stol(result_or_error));
                    break;
                default:
                    break;
            }
        }
        // cur_table.delete_record(cursor);
        cursor.delete_record();
        cur_table.add_record(currec);
        // Moving to the next table record
    }

    // Closing the table and exiting
    std::stringstream res;
    res << "Updated table " << table_name.data();
    result_or_error.clear();
    result_or_error = res.str();
    return true;
}

bool execute_select(std::vector<std::string> field_names,
                    std::string table_name, WhereCondition where_condition,
                    std::string &result_or_error) {
    // Opening the table created
    table cur_table(table_name);
    cur_table.read_header();
    // Getting table field names if necessary
    std::string error;
    if ((field_names.size() == 0) &&
        (!get_table_field_names(cur_table, field_names, error))) {
        result_or_error = error;
        return false;
    }
    if (field_names.size() == 0) {
        std::stringstream roerr;
        roerr << "The " << table_name.data() << " table does not have fields"
              << std::endl;
        result_or_error.clear();
        result_or_error = roerr.str();
        return false;
    }

    // Looping through records
    for (auto cur = cur_table.begin(); cur != cur_table.end(); ++cur) {
        // Checking if the current record meets the "where" condition
        std::string where_result;
        auto prxy = *cur;
        if (!where_pipeline(cur_table, &prxy, where_condition, where_result)) {
            return false;
        } else if ("1" != where_result) {
            continue;
        }

        // Getting current record data
        auto cors = cur_table.get_correlations();
        std::string value;
        for (auto cur_fld : field_names) {
            auto type = cur_table.get_field_type(cur_fld);
            std::size_t plc;
            for (plc = 0; plc < cors.size(); ++plc) {
                if (cors[plc].first == cur_fld) {
                    break;
                }
            }
            try {
                switch (type) {
                    case Text:
                        value += (*cur)[plc]->get_text();
                        break;

                    case Long:
                        value += std::to_string((*cur)[plc]->get_long());
                        break;

                    case Bool:
                        throw "kill me";
                }
                value += "\t";
            } catch (std::exception &e) {
                std::stringstream roerr;
                roerr << "Failed to read the " << cur_fld
                      << " field value from the " << table_name.data()
                      << " table: " << e.what() << std::endl;
                result_or_error.clear();
                result_or_error = roerr.str();

                return false;
            }
        }
        result_or_error += value + "\n";
    }

    return true;
}