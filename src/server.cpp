#include <iostream>

#include "exceptions/exceptions.hpp"
#include "rec_desc/runner.hpp"
#include "rec_desc/scanner.hpp"
#include "sockets/sockets.hpp"

int main() {
    ModelSQL::UnServerSocket socko_mode("socket", -1);
    ModelSQL::BaseSocket cl_sock;
    try {
        cl_sock = socko_mode.accept();

    } catch (serv_except::accept_error &e) {
        std::cerr << "\t" << e.what() << std::endl;
        abort();
    }
    std::cout << "new connection!!" << std::endl;
    std::string str;
    try {
        str = cl_sock.get_string();
    } catch (serv_except::recv_error &e) {
        std::cerr << "\t" << e.what() << std::endl;
    } 
    auto lexms = parse(str);
    Parser parser;
    std::string ans = parser.analyze(lexms);
    cl_sock.put_string(ans);
}