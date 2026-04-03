#include "log.hpp"

#include <chrono>
#include <filesystem>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

bool Logger::initialize(const std::string& logDir, const std::string& appName, Mode mode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) return true;

    m_logDir   = logDir;
    m_appName  = appName;
    m_mode     = mode;

    try {
        if (mode == Mode::Asynchronous)
            setupAsyncLogging();
        setupDefaultLoggers();
        m_initialized = true;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Logger::setupAsyncLogging() {
    spdlog::init_thread_pool(8192, 1);
}

void Logger::setupDefaultLoggers() {
    std::vector<spdlog::sink_ptr> sinks;

    if (m_consoleEnabled) {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(consoleSink);
    }

    if (m_fileEnabled && !m_logDir.empty()) {
        std::filesystem::create_directories(m_logDir);
        std::string logPath = m_logDir + "/" + m_appName + ".log";
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logPath, m_maxFileSize, m_maxFiles);
        sinks.push_back(fileSink);
    }

    if (m_mode == Mode::Asynchronous) {
        m_defaultLogger = std::make_shared<spdlog::async_logger>(
            m_appName, sinks.begin(), sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
    } else {
        m_defaultLogger = std::make_shared<spdlog::logger>(
            m_appName, sinks.begin(), sinks.end());
    }

    spdlog::register_logger(m_defaultLogger);
    spdlog::set_default_logger(m_defaultLogger);
}

void Logger::cleanup() {
    std::lock_guard<std::mutex> lock(m_mutex);
    spdlog::shutdown();
    m_initialized = false;
}

Logger::~Logger() {
    cleanup();
}

void Logger::setLevel(Level level) {
    m_currentLevel = level;
    if (m_defaultLogger)
        m_defaultLogger->set_level(static_cast<spdlog::level::level_enum>(level));
}

void Logger::enableConsole(bool enable) { m_consoleEnabled = enable; }
void Logger::enableFile(bool enable)    { m_fileEnabled = enable; }

void Logger::setFileRotation(size_t maxFileSize, size_t maxFiles) {
    m_maxFileSize = maxFileSize;
    m_maxFiles    = maxFiles;
}

void Logger::setPattern(const std::string& pattern) {
    if (m_defaultLogger)
        m_defaultLogger->set_pattern(pattern);
}

void Logger::flush() {
    if (m_defaultLogger) m_defaultLogger->flush();
}

void Logger::setFlushInterval(int seconds) {
    spdlog::flush_every(std::chrono::seconds(seconds));
}

// 简单字符串接口
void Logger::trace(const std::string& msg, const std::string&) {
    if (m_defaultLogger) m_defaultLogger->trace(msg);
}
void Logger::debug(const std::string& msg, const std::string&) {
    if (m_defaultLogger) m_defaultLogger->debug(msg);
}
void Logger::info(const std::string& msg, const std::string&) {
    if (m_defaultLogger) m_defaultLogger->info(msg);
}
void Logger::warning(const std::string& msg, const std::string&) {
    if (m_defaultLogger) m_defaultLogger->warn(msg);
}
void Logger::error(const std::string& msg, const std::string&) {
    if (m_defaultLogger) m_defaultLogger->error(msg);
}
void Logger::critical(const std::string& msg, const std::string&) {
    if (m_defaultLogger) m_defaultLogger->critical(msg);
}

// 条件日志
void Logger::traceIf(bool c, const std::string& msg, const std::string& cat)    { if (c) trace(msg, cat); }
void Logger::debugIf(bool c, const std::string& msg, const std::string& cat)    { if (c) debug(msg, cat); }
void Logger::infoIf(bool c, const std::string& msg, const std::string& cat)     { if (c) info(msg, cat); }
void Logger::warningIf(bool c, const std::string& msg, const std::string& cat)  { if (c) warning(msg, cat); }
void Logger::errorIf(bool c, const std::string& msg, const std::string& cat)    { if (c) error(msg, cat); }
void Logger::criticalIf(bool c, const std::string& msg, const std::string& cat) { if (c) critical(msg, cat); }

// 分类日志器
std::shared_ptr<spdlog::logger> Logger::getLogger(const std::string& name) {
    auto logger = spdlog::get(name);
    if (!logger) {
        logger = m_defaultLogger->clone(name);
        spdlog::register_logger(logger);
    }
    return logger;
}

void Logger::removeLogger(const std::string& name) {
    spdlog::drop(name);
}

// Timer
Logger::Timer::Timer(const std::string& operation, Level level)
    : m_operation(operation), m_level(level), m_stopped(false) {
    m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

Logger::Timer::~Timer() {
    if (!m_stopped) stop();
}

void Logger::Timer::stop() {
    if (m_stopped) return;
    m_stopped = true;
    long long ms = elapsed();
    Logger::instance().info("[Timer] " + m_operation + " took " + std::to_string(ms) + "ms");
}

long long Logger::Timer::elapsed() const {
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    return now - m_startTime;
}

std::unique_ptr<Logger::Timer> Logger::startTimer(const std::string& operation, Level level) {
    return std::make_unique<Timer>(operation, level);
}
