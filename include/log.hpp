/**
 * @file     Logger.h
 * @brief    日志封装库-基于spdlog
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>
#include <mutex>

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>

class Logger
{
public:
    enum class Level {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Critical = 5,
        Off = 6
    };

    enum class Mode {
        Synchronous,
        Asynchronous
    };

    static Logger& instance();

    bool initialize(const std::string& logDir = "",
                   const std::string& appName = "App",
                   Mode mode = Mode::Asynchronous);

    void cleanup();
    void setLevel(Level level);
    Level level() const { return m_currentLevel; }
    void enableConsole(bool enable = true);
    void enableFile(bool enable = true);
    void setFileRotation(size_t maxFileSize = 10 * 1024 * 1024, size_t maxFiles = 10);
    void setPattern(const std::string& pattern);
    void flush();
    void setFlushInterval(int seconds);

    // 简单字符串接口
    void trace(const std::string& message, const std::string& category = "");
    void debug(const std::string& message, const std::string& category = "");
    void info(const std::string& message, const std::string& category = "");
    void warning(const std::string& message, const std::string& category = "");
    void error(const std::string& message, const std::string& category = "");
    void critical(const std::string& message, const std::string& category = "");

    // 格式化接口
    template<typename... Args>
    void trace(const std::string& format, Args&&... args) {
        if (m_defaultLogger && m_currentLevel <= Level::Trace)
            m_defaultLogger->trace(fmt::runtime(format), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        if (m_defaultLogger && m_currentLevel <= Level::Debug)
            m_defaultLogger->debug(fmt::runtime(format), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        if (m_defaultLogger && m_currentLevel <= Level::Info)
            m_defaultLogger->info(fmt::runtime(format), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(const std::string& format, Args&&... args) {
        if (m_defaultLogger && m_currentLevel <= Level::Warning)
            m_defaultLogger->warn(fmt::runtime(format), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        if (m_defaultLogger && m_currentLevel <= Level::Error)
            m_defaultLogger->error(fmt::runtime(format), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(const std::string& format, Args&&... args) {
        if (m_defaultLogger && m_currentLevel <= Level::Critical)
            m_defaultLogger->critical(fmt::runtime(format), std::forward<Args>(args)...);
    }

    // 条件日志
    void traceIf(bool condition, const std::string& message, const std::string& category = "");
    void debugIf(bool condition, const std::string& message, const std::string& category = "");
    void infoIf(bool condition, const std::string& message, const std::string& category = "");
    void warningIf(bool condition, const std::string& message, const std::string& category = "");
    void errorIf(bool condition, const std::string& message, const std::string& category = "");
    void criticalIf(bool condition, const std::string& message, const std::string& category = "");

    // 分类日志器
    std::shared_ptr<spdlog::logger> getLogger(const std::string& name);
    void removeLogger(const std::string& name);

    // 性能计时
    class Timer {
    public:
        Timer(const std::string& operation, Level level = Level::Info);
        ~Timer();
        void stop();
        long long elapsed() const;
    private:
        std::string m_operation;
        Level m_level;
        long long m_startTime;
        bool m_stopped;
    };

    std::unique_ptr<Timer> startTimer(const std::string& operation, Level level = Level::Info);

private:
    explicit Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void setupDefaultLoggers();
    void setupAsyncLogging();

    std::shared_ptr<spdlog::logger> m_defaultLogger;
    std::shared_ptr<spdlog::logger> m_consoleLogger;
    std::shared_ptr<spdlog::logger> m_fileLogger;

    std::string m_logDir;
    std::string m_appName;
    Mode m_mode = Mode::Asynchronous;
    Level m_currentLevel = Level::Info;
    bool m_initialized = false;
    bool m_consoleEnabled = true;
    bool m_fileEnabled = true;
    size_t m_maxFileSize = 10 * 1024 * 1024;
    size_t m_maxFiles = 10;

    mutable std::mutex m_mutex;
};

// 宏定义
#define LOG_TRACE(msg, ...)    Logger::instance().trace(msg, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...)    Logger::instance().debug(msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...)     Logger::instance().info(msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...)  Logger::instance().warning(msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...)    Logger::instance().error(msg, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) Logger::instance().critical(msg, ##__VA_ARGS__)

#define LOG_TRACE_IF(cond, msg)    Logger::instance().traceIf(cond, msg)
#define LOG_DEBUG_IF(cond, msg)    Logger::instance().debugIf(cond, msg)
#define LOG_INFO_IF(cond, msg)     Logger::instance().infoIf(cond, msg)
#define LOG_WARNING_IF(cond, msg)  Logger::instance().warningIf(cond, msg)
#define LOG_ERROR_IF(cond, msg)    Logger::instance().errorIf(cond, msg)
#define LOG_CRITICAL_IF(cond, msg) Logger::instance().criticalIf(cond, msg)

#define LOG_TIMER(op)       auto _timer = Logger::instance().startTimer(op)
#define LOG_TIMER_DEBUG(op) auto _timer = Logger::instance().startTimer(op, Logger::Level::Debug)

#endif // LOGGER_H
