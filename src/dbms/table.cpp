#include "table.hpp"

#include "../exceptions/exceptions.hpp"

std::string table::get_next_token() {
    std::string retval;
    char cur_char;
    while (tab_str.peek() == '\0') {
        tab_str.seekg(1, std::fstream::cur);
    }
    do {
        tab_str.read(&cur_char, sizeof(cur_char));
        retval += cur_char;
    } while (cur_char != '\0');
    while (tab_str.peek() == '\0') {
        tab_str.seekg(1, std::fstream::cur);
    }
    return retval;
}

table::table(std::string name) {
    tab_str.open(name, std::fstream::in | std::fstream::out);
    if (!tab_str.is_open()) {
        throw std::runtime_error("file opeinig error");
    }
    // tab_str.exceptions(std::fstream::failbit);
}

table::~table() { tab_str.close(); }

void table::set_correlations(std::vector<correlation> new_cors) {
    correlations = new_cors;
}

std::vector<correlation> table::get_correlations() { return correlations; }

void table::add_record(record rec) {
    // std::cout << "failbit is " << tab_str.fail() << std::endl;
    tab_str.seekp(0, std::fstream::end);
    tab_str.seekg(0, std::fstream::end);
    // std::cout << "failbit is " << tab_str.fail() << std::endl;

    // std::cout << "offset is " << tab_str.tellp() << std::endl;
    auto i = 0;
    std::string cur_string;
    for (auto &cur : correlations) {
        switch (cur.second) {
            case Long:
                cur_string = std::to_string(rec[i]->get_long());
                break;
            case Text:
                cur_string = rec[i]->get_text();
                break;
            case Bool:
                cur_string = std::to_string(rec[i]->get_bool());
                break;
            default:
                break;
        }
        // std::cout << "writing: " << cur_string << std::endl;
        tab_str.write(cur_string.c_str(), cur_string.size());
        // std::cout << errno << std::endl;
        tab_str.write("\0", sizeof('\0'));
        ++i;
    }
    tab_str.flush();
}

long_node::long_node(int64_t _val) : val{_val} {}

int64_t long_node::get_long() { return val; }

std::string long_node::get_text() { throw db_except::mismatched_types(); }

bool long_node::get_bool() { throw db_except::mismatched_types(); }

text_node::text_node(std::string _val) : val{_val} {}

int64_t text_node::get_long() { throw db_except::mismatched_types(); }

std::string text_node::get_text() { return val; }

bool text_node::get_bool() { throw db_except::mismatched_types(); }

bool_node::bool_node(bool _val) : val{_val} {}

int64_t bool_node::get_long() { throw db_except::mismatched_types(); }

std::string bool_node::get_text() { throw db_except::mismatched_types(); }

bool bool_node::get_bool() { return val; }

void table::read_header() {
    std::string to_parse;
    correlations.clear();
    tab_str.seekg(0, std::fstream::beg);
    tab_str.seekp(0, std::fstream::beg);
    to_parse = get_next_token();
    auto shit = std::stoi(to_parse);
    for (auto k = 0; k < shit; ++k) {
        auto field_name = get_next_token();
        to_parse = get_next_token();
        // std::cout << field_name << "\t" << std::endl;
        field_name.resize(field_name.size() - 1);
        switch (std::stoi(to_parse)) {
            case Long:
                correlations.emplace_back(field_name, Long);
                break;
            case Text:
                correlations.emplace_back(field_name, Text);
                break;
            case Bool:
                correlations.emplace_back(field_name, Bool);
                break;
        }
    }
    header_offset = tab_str.tellg();
}

void table::write_header() {
    std::string proxy;
    proxy = std::to_string(correlations.size());
    tab_str.write(proxy.c_str(), proxy.size());
    tab_str.write("\0", sizeof('\0'));
    for (auto cur : correlations) {
        // proxy = std::to_string(cur.first);
        tab_str.write(cur.first.c_str(), cur.first.size());
        tab_str.write("\0", sizeof('\0'));
        proxy = std::to_string(cur.second);
        tab_str.write(proxy.c_str(), proxy.size());
        tab_str.write("\0", sizeof('\0'));
    }
    header_offset = tab_str.tellp();
}

record table::get_record(std::size_t place) {
    std::string to_parse;
    tab_str.seekg(header_offset, std::fstream::beg);
    for (auto i = 0UL; i < place; ++i) {
        for (auto cur : correlations) {
            get_next_token();
        }
    }
    record retval;
    for (auto cur : correlations) {
        to_parse = "";
        to_parse = get_next_token();
        if (cur.second == Long) {
            retval.emplace_back(new long_node(std::stol(to_parse)));
        } else if (cur.second == Text) {
            retval.emplace_back(new text_node(to_parse));
        } else if (cur.second == Bool) {
            retval.emplace_back(new bool_node(std::stol(to_parse)));
        } else {
            throw db_except::corrupted_table();
        }
    }
    return retval;
}

void table::delete_record(std::size_t place) {
    tab_str.seekg(header_offset, std::fstream::beg);
    for (auto i = 0UL; i < place; ++i) {
        for (auto cur : correlations) {
            get_next_token();
        }
    }
    for (auto cur : correlations) {
        while (tab_str.peek() != '\0') {
            tab_str.write("\0", sizeof('\0'));
        }
        tab_str.seekp(1, std::fstream::cur);
    }
}

fld_types table::get_field_type(std::string field_name) {
    for (auto cur : correlations) {
        if (cur.first == field_name) {
            return cur.second;
        }
    }
    throw db_except::no_field_found(field_name);
}

void table::drop() {
    ::unlink(tab_name.c_str());
    // TODO(me): add unlink handling
}

record table::iterator::operator*() {
    record retval;
    std::string to_parse;
    auto pos = this_tab.tab_str.tellg();
    for (auto cur : this_tab.correlations) {
        to_parse = "";
        to_parse = this_tab.get_next_token();
        if (cur.second == Long) {
            retval.emplace_back(new long_node(std::stol(to_parse)));
        } else if (cur.second == Text) {
            retval.emplace_back(new text_node(to_parse));
        } else if (cur.second == Bool) {
            retval.emplace_back(new bool_node(std::stol(to_parse)));
        } else {
            throw db_except::corrupted_table();
        }
    }
    this_tab.tab_str.seekg(pos, std::fstream::beg);
    this_tab.tab_str.seekp(pos, std::fstream::beg);
    return retval;
}

std::size_t table::iterator::get_place() const { return cur_place; }

table::iterator &table::iterator::operator++() {
    for (auto cur : this_tab.correlations) {
        this_tab.get_next_token();
    }
    ++cur_place;
    cur_offset = this_tab.tab_str.tellg();
    return *this;
}

const table::iterator table::iterator::operator++(int) {
    auto retval = *this;
    ++(*this);
    return retval;
}

bool table::iterator::operator!=(iterator other) {
    return cur_offset != other.cur_offset;
}

void table::iterator::delete_record() {
    auto plc = this_tab.tab_str.tellg();
    for (auto cur : this_tab.correlations) {
        while (this_tab.tab_str.peek() != '\0') {
            this_tab.tab_str.write("\0", sizeof('\0'));
        }
        this_tab.tab_str.seekp(1, std::fstream::cur);
    }
    this_tab.tab_str.seekg(plc);
    this_tab.tab_str.seekp(plc);
    // cur_offset = this_tab.tab_str.tellg();
}

table::iterator::iterator(table &tabl)
    : this_tab{tabl}, cur_place(0), cur_offset(0) {}

table::iterator table::begin() {
    iterator retval{*this};
    std::string to_parse;
    tab_str.seekg(0, std::ios::beg);
    tab_str.seekp(0, std::ios::beg);
    to_parse = get_next_token();
    auto shit = std::stoi(to_parse);
    for (auto k = 0; k < shit; ++k) {
        auto field_name = get_next_token();
        to_parse = get_next_token();
        // std::cout << field_name << "\t" << std::endl;
    }
    retval.cur_offset = tab_str.tellg();
    return retval;
}

table::iterator table::end() {
    iterator retval{*this};
    auto save = tab_str.tellg();
    tab_str.seekg(0, std::fstream::end);
    tab_str.seekp(0, std::fstream::end);
    retval.cur_offset = tab_str.tellg();
    tab_str.seekg(save, std::fstream::beg);
    tab_str.seekp(save, std::fstream::beg);
    return retval;
}