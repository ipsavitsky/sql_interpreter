#include "sockets/sockets.hpp"

int main(){
    ModelSQL::UnClientSocket cl_sock("socket");
    cl_sock.connect();
    std::string query;
    std::cin >> query; 
    cl_sock.put_string(query);
    query = cl_sock.get_string();
    std::cout << query;
}