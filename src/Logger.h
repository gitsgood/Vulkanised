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

#include "Utilities.h"

namespace Color
{
    constexpr const char* Reset{ "\033[0m" };                                   // Resets console output to default color
    constexpr const char* Error{ "\033[48;2;230;0;38;38;2;0;230;192m" };        // Green-ish on red background
    constexpr const char* Warning{ "\033[48;2;230;226;0;38;2;0;4;230m" };       // Blue on yellow background
    constexpr const char* Highlight{ "\033[48;2;0;183;235;38;2;235;52;0m" };    // Red-ish on cyan background
    constexpr const char* FunctionNames{ "\033[48;2;255;255;255;38;2;0;0;0m" }; // Black on white background
    constexpr const char* Timestamp{ "\033[48;2;150;75;0;38;2;7;35;55m" };      // Blue-ish on brown background
}

class Logger
{
public:
    static std::shared_ptr<Logger> getLoggerInstance()
    {
        static std::shared_ptr<Logger> loggerSingleton{ new Logger()};
        return loggerSingleton;
    }

    enum class LogType
    {
        LOG = 0,
        HIGHLIGHT,
        WARNING,
        ERR
    };

    void logText(std::string_view input, LogType inColor = LogType::LOG, std::source_location callSite = std::source_location::current());

    std::string getTimestamp();

private:
    // This is supposed to lock access to the logger if one thread accesses it (so for example, another thread wont access it at the same time).
    std::mutex m_Mutex;

    // One ofstream instance.
    std::ofstream m_File{ std::filesystem::path(Utilities::PATH) / "VulkanisedLog.log", std::ios::app };

	// Private constructors to strictly control instantiation of the class.
    Logger()
    {
        //std::cout << Color::Error << "ERROR: System failure! " << Color::Reset << std::endl;
        //std::cout << Color::Warning << "WARNING: Low disk space. " << Color::Highlight <<std::endl;
        logText("I breath...", LogType::ERR);
        logText("I shall prepare my log file...", LogType::HIGHLIGHT);
        logText("Thou hast been warned!", LogType::WARNING);

    }

    // Delete the copy constructor and assignment operator, since its a static class and shouldn't exist anyways.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Delete move operations, since its a static class and shouldn't exist anyways.
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};

#endif // !LOGGER_H
