#ifndef GENIE_LOGGER_H_
#define GENIE_LOGGER_H_

// Usage example:
//     Logger::init("app.log");
//     Logger &logger = Logger::instance();
//     logger.out("test");

#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include "util/exceptions.h"

namespace genie {

class Logger {
   public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    static void init(const std::string& filename) {
        std::lock_guard<std::mutex> lock(instance().mtx);
        if (instance().f.is_open()) {
            throw std::runtime_error{"Logger already initialized"};
        }
        instance().f.open(filename, std::ios::binary | std::ios::app);
    }

    void out(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        time_t t = time(nullptr);
        // if (t == ((time_t) - 1)) { UTILS_DIE("time failed"); }
        std::stringstream ss;
        struct tm timeinfo {};
        // ISO 8601 format: 2007-04-05T14:30:21Z
        ss << "[" << std::put_time(gmtime_r(&t, &timeinfo), "%Y-%m-%dT%H:%M:%SZ") << "] " << msg << std::endl;
        f << ss.str();
        f.flush();
    }

    void flush() {
        std::lock_guard<std::mutex> lock(mtx);
        f.flush();
    }

   private:
    std::mutex mtx;
    std::ofstream f;

    Logger() = default;
    ~Logger() = default;
    Logger(const Logger& logger) = delete;
    Logger& operator=(const Logger& logger) = delete;
};

}  // namespace genie

#endif  // GENIE_LOGGER_H_
