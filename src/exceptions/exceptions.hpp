#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <string>

namespace db_except {
class mismatched_types : std::exception {
   public:
    const char *what() const noexcept override;
};

class corrupted_table : std::exception {
   public:
    const char *what() const noexcept override;
};

class no_field_found : std::exception {
   private:
    std::string name;

   public:
    explicit no_field_found(std::string fld_name);
    const char *what() const noexcept override;
};

}  // namespace db_except

namespace serv_except {
class unknown_exception : std::exception {
   public:
    const char *what() const noexcept override;
};

class incorrect_value : std::exception {
   public:
    const char *what() const noexcept override;
};

class send_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit send_error(int err);
    const char *what() const noexcept override;
};

class recv_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit recv_error(int err);
    const char *what() const noexcept override;
};

class bind_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit bind_error(int err);
    const char *what() const noexcept override;
};

class listen_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit listen_error(int err);
    const char *what() const noexcept override;
};

class accept_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit accept_error(int err);
    const char *what() const noexcept override;
};

class socket_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit socket_error(int err);
    const char *what() const noexcept override;
};

class convert_error : std::exception {
   private:
    int err_msg;  // errno stuff
   public:
    explicit convert_error(int err);
    const char *what() const noexcept override;
};

}  // namespace serv_except

namespace parse_except {

class unexpected_token : std::exception {
   private:
    std::string expected_token;

   public:
    explicit unexpected_token(std::string tok);
    const char *what() const noexcept override;
};

class illegal_token : std::exception {
   private:
    std::string token;

   public:
    explicit illegal_token(std::string tok);
    const char *what() const noexcept override;
};

}  // namespace parse_except

#endif  // EXCEPTION_H_