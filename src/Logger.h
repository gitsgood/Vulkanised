#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <source_location>
#include <memory>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <format>
#include <chrono>

#include "glaze/glaze.hpp"

#include "Utilities.h"

// The macros we will use henceforth to log ANYTHING. We really did learn from the best
#define LOGGER(type, fmt, ...)  Logger::getLoggerInstance()->logText(type, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG(fmt, ...)           Logger::getLoggerInstance()->logText(Logger::LogType::LOG, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOGH(fmt, ...)          Logger::getLoggerInstance()->logText(Logger::LogType::HIGHLIGHT, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...)          Logger::getLoggerInstance()->logText(Logger::LogType::WARNING, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...)          Logger::getLoggerInstance()->logText(Logger::LogType::ERR, std::source_location::current(), fmt, ##__VA_ARGS__)

namespace Color
{
    constexpr const char* Reset{ "\033[0m" };                                   // Resets console output to default color
    constexpr const char* Error{ "\033[48;2;230;0;38;38;2;0;230;192m" };        // Green-ish on red background
    constexpr const char* Warning{ "\033[48;2;230;226;0;38;2;0;4;230m" };       // Blue on yellow background
    constexpr const char* Highlight{ "\033[48;2;0;183;235;38;2;235;52;0m" };    // Red-ish on cyan background
    constexpr const char* FunctionNames{ "\033[48;2;255;255;255;38;2;0;0;0m" }; // Black on white background
    constexpr const char* Timestamp{ "\033[48;2;150;75;0;38;2;7;35;55m" };      // Blue-ish on brown background
}

/*
* Logger class. No matter what, the logger itself can never use the log macros for itself. Macros are for everyone else.
* Thank you for your attention to this matter...
*/
class Logger
{
public:
    static std::shared_ptr<Logger> getLoggerInstance()
    {
        static std::shared_ptr<Logger> loggerSingleton{ new Logger() };
        return loggerSingleton;
    }

    ~Logger()
    {
        //writeSettingsFile();  // For now we will only write the settings when we try to read them (if they dont exist, and once). If we make an editor we can refactor this later.
        logText(LogType::HIGHLIGHT, std::source_location::current(), "Goodnight logger...");
        m_File << Utilities::LOG_SEPARATION;
    }

    enum class LogType
    {
        LOG = 0,
        HIGHLIGHT,
        WARNING,
        ERR
    };

    std::string getTimestamp();

    // I want to be able to log things with modern formatting abilities. This here function takes care of it.
    template <typename... Args>
    void logText(LogType type, std::source_location loc, std::string_view fmt, Args&&... args)
    {
        std::string formattedMessage = std::vformat(fmt, std::make_format_args(args...));
        logInternal(formattedMessage, type, loc);
    }

    // A simple overload in case I don't feel like specifying what LogType I want, without needing to mess with the sensitive parameter structure...
    template <typename... Args>
    void logText(std::source_location loc, std::string_view fmt, Args&&... args)
    {
        // We simply forward everything to the first version, adding a default of LogType::LOG
        logText(LogType::LOG, loc, fmt, std::forward<Args>(args)...);
    }

private:
    // This is supposed to lock access to the logger if one thread accesses it (so for example, another thread wont access it at the same time).
    std::mutex m_Mutex;

    // One ofstream instance.
    std::ofstream m_File;

    // Settings state, stored in struct (which is also used for json parsing)
    struct LogSettings {
        bool logToFile{ true };
        bool deleteLogFileAtStart{ true };
        bool logToConsole{ true };
    } m_LoggerSettings;

    // Methods for our eyes only...
    void readSettingsFile();
    void writeSettingsFile();

    // For debugging purposes. Could eventually remove this completely down the line.
    void printBooleans();

    void logInternal(const std::string& input, LogType inColor, std::source_location callSite);

	// Private constructors to strictly control instantiation of the class.
    Logger()
    {
        readSettingsFile();
        if (!m_LoggerSettings.deleteLogFileAtStart)
        {
            m_File.open(std::filesystem::path(Utilities::PATH) / "VulkanisedLog.log", std::ios::app);
        }
        else
        {
            m_File.open(std::filesystem::path(Utilities::PATH) / "VulkanisedLog.log");
        }
        //printBooleans();
        logText(LogType::HIGHLIGHT, std::source_location::current(), "Logger initialised...");
    }

    // Delete the copy constructor and assignment operator, since its a static class and shouldn't exist anyways.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Delete move operations, since its a static class and shouldn't exist anyways.
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};

#endif // !LOGGER_H
