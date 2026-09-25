#include "Logger.h"

using namespace Vulkanised;

std::string Logger::getTimestamp()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - Utilities::Time::getStartTime());

    double elapsedSeconds = elapsed.count() / 1000.0;

    std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now();

    return std::format("[{:%H:%M:%S}] [{:.6f}s] ", systemNow, elapsedSeconds);
}

void Logger::readSettingsFile()
{
    std::filesystem::path settingsPath{ std::filesystem::path(Utilities::File::c_Path) / Utilities::File::c_LogSettingsFileName };
    std::string jsonBuffer{ "" };
    glz::error_ctx settingsReadFailure = glz::read_file_json(m_LoggerSettings, settingsPath.string(), jsonBuffer);
    if (!settingsReadFailure) { logText(LogType::HIGHLIGHT, std::source_location::current(), "Succesfully read the json..."); }
    else 
    { 
        writeSettingsFile(); 
        logText(LogType::WARNING, std::source_location::current(), "Logger settings not found, creating default...");
    }
    //logText(LogType::HIGHLIGHT, std::source_location::current(), "{}", jsonBuffer);
}

void Logger::writeSettingsFile()
{
    std::filesystem::path settingsPath{ std::filesystem::path(Utilities::File::c_Path) / Utilities::File::c_LogSettingsFileName };
    std::string buffer = glz::write_json(m_LoggerSettings).value_or("error");
    std::ofstream jsonFile;
    jsonFile.open(settingsPath);
    if (!jsonFile) { logText(LogType::ERR, std::source_location::current(), "Couldn't write the json somehow..."); }
    jsonFile << buffer;
    jsonFile.close();
}

void Logger::printBooleans()
{
    std::stringstream finalMessage;
    finalMessage << "\n";
    finalMessage << (m_LoggerSettings.logToFile ? "logToFile: true\n" : "logToFile: false\n");
    finalMessage << (m_LoggerSettings.deleteLogFileAtStart ? "deleteLogFileAtStart: true\n" : "deleteLogFileAtStart: false\n");
    finalMessage << (m_LoggerSettings.logToConsole ? "logToConsole: true\n" : "logToConsole: false\n");

    logText(LogType::HIGHLIGHT, std::source_location::current(), finalMessage.str());
}

void Logger::logInternal(const std::string& input, LogType inColor, std::source_location callSite)
{
    // Early exit: If logging is totally disabled, don't even lock the mutex
    if (!m_LoggerSettings.logToConsole && !m_LoggerSettings.logToFile) return;


    std::lock_guard<std::mutex> lock(m_Mutex);

    const char* colorCode = Color::Reset;
    const char* prefix = " ";

    switch (inColor)
    {
    case LogType::LOG:
        colorCode = Color::Reset;
        break;
    case LogType::HIGHLIGHT:
        colorCode = Color::Highlight;
        break;
    case LogType::TIMING:
        colorCode = Color::TimedFunc;
        prefix = " TIMING: ";
        break;
    case LogType::WARNING:
        colorCode = Color::Warning;
        prefix = " WARNING: ";
        break;
    case LogType::ERR:
        colorCode = Color::Error;
        prefix = " ERROR: ";
        break;
    default:
        colorCode = Color::Reset;
        break;
    }

    // Building the function name string
    std::string functionName = std::format("[ {} ]", callSite.function_name());

    // Building the message
    std::string message = std::format("{}{}", prefix, input);

    // Build the timestamp, once
    std::string timestamp = getTimestamp();

    // Log to console
#ifdef NDEBUG
    // Do nothing, we have no console without debug mode
#else
    if (m_LoggerSettings.logToConsole)
    {
        //std::cout << Color::Timestamp << timestamp << Color::FunctionNames << functionName << Color::Reset << colorCode << message << Color::Reset << "\n";
        std::println("{}{}{}{}{}{}{}{}", Color::Timestamp, timestamp, Color::FunctionNames, functionName, Color::Reset, colorCode, message, Color::Reset);
    }
#endif

    // Output log to a file
    if (m_LoggerSettings.logToFile)
    {
        //m_File << timestamp << functionName << message << "\n";
        std::println(m_File, "{}{}{}", timestamp, functionName, message);
    }
}
