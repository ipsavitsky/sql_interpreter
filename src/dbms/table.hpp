#ifndef _Table_h_
#define _Table_h_

#include <unistd.h>

#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class node {
   public:
    virtual ~node() = default;
    virtual int64_t get_long() = 0;
    virtual std::string get_text() = 0;
    virtual bool get_bool() = 0;
};

class long_node : public node {
   private:
    int64_t val;

   public:
    explicit long_node(int64_t _val);
    int64_t get_long() override;
    std::string get_text() override;
    bool get_bool() override;
};

class text_node : public node {
   private:
    std::string val;

   public:
    explicit text_node(std::string _val);
    int64_t get_long() override;
    std::string get_text() override;
    bool get_bool() override;
};

class bool_node : public node {
   private:
    bool val;

   public:
    explicit bool_node(bool _val);
    int64_t get_long() override;
    std::string get_text() override;
    bool get_bool() override;
};

enum fld_types { Long, Text, Bool };
using correlation = std::pair<std::string, fld_types>;
using record = std::vector<std::shared_ptr<node>>;

class table {
   private:
    std::streampos header_offset;
    std::string tab_name;
    std::fstream tab_str;
    std::vector<correlation> correlations;

    std::string get_next_token();

   public:
    class iterator {
       private:
        table &this_tab;
        std::size_t cur_place;

       public:
        std::streampos cur_offset;

        iterator(table &);
        record operator*();
        std::size_t get_place() const;
        iterator &operator++();
        const iterator operator++(int);
        bool operator!=(iterator other);
        void delete_record();
    };
    explicit table(std::string name);
    ~table();

    void read_header();
    void write_header();

    void set_correlations(std::vector<correlation> new_cors);
    std::vector<correlation> get_correlations();

    fld_types get_field_type(std::string field_name);
    record get_record(std::size_t place);
    void add_record(record rec);
    void delete_record(std::size_t place);
    void drop();
    iterator begin();
    iterator end();
};

#endif