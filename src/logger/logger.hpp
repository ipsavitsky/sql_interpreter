#include <fstream>
#include <iostream>
#include <vector>

class Logger {
   public:
    virtual ~Logger() = default;
    virtual void info(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    virtual void warning(const std::string& message) = 0;
};

class FileLogger : public Logger {
   private:
    std::ofstream fstr;

   public:
    FileLogger() = delete;
    explicit FileLogger(const std::string& filename);
    ~FileLogger() override;
    void info(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
};

class StderrLogger : public Logger {
   public:
    void info(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
};

class CompositeLogger : public Logger {
   private:
    std::vector<Logger*> loggers;

   public:
    void info(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
    void add(Logger* logger);
};