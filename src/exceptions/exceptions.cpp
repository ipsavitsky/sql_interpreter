

#include "exceptions.hpp"

#include <cstring>

const char *db_except::mismatched_types::what() const noexcept {
    return "mismatched types";
}

const char *db_except::corrupted_table::what() const noexcept {
    return "table is corrupted";
}

db_except::no_field_found::no_field_found(std::string fld_name)
    : name{fld_name} {}

const char *db_except::no_field_found::what() const noexcept {
    const char *res =
        strdup(("field " + name + " not found").c_str());
    return res;
}

using namespace serv_except;

send_error::send_error(int err) : err_msg{err} {}

const char *send_error::what() const noexcept { return strerror(err_msg); }

recv_error::recv_error(int err) : err_msg{err} {}

const char *recv_error::what() const noexcept { return strerror(err_msg); }

bind_error::bind_error(int err) : err_msg{err} {}

const char *bind_error::what() const noexcept { return strerror(err_msg); }

listen_error::listen_error(int err) : err_msg{err} {}

const char *listen_error::what() const noexcept { return strerror(err_msg); }

accept_error::accept_error(int err) : err_msg{err} {}

const char *accept_error::what() const noexcept { return strerror(err_msg); }

socket_error::socket_error(int err) : err_msg{err} {}

const char *socket_error::what() const noexcept { return strerror(err_msg); }

convert_error::convert_error(int err) : err_msg{err} {}

const char *convert_error::what() const noexcept { return strerror(err_msg); }

parse_except::unexpected_token::unexpected_token(std::string tok)
    : expected_token{tok} {}

const char *parse_except::unexpected_token::what() const noexcept {
    const char *res =
        strdup(("unexpected token\n" + expected_token + " expected").c_str());
    return res;
}

parse_except::illegal_token::illegal_token(std::string tok) : token{tok} {}

const char *parse_except::illegal_token::what() const noexcept {
    const char *res = strdup(("illegal token\n" + token).c_str());
    return res;
}
