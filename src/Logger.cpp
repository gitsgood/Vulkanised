#include "Logger.h"

void Logger::logText(std::string_view input, LogType inColor, std::source_location callSite)
{
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

	// Log to console
#ifdef NDEBUG
	// Do nothing, we have no console without debug mode
#else
	std::cout << Color::Timestamp << getTimestamp() << Color::FunctionNames << functionName << Color::Reset << colorCode << message << Color::Reset << "\n";
#endif

	// Output log to a file
	m_File << getTimestamp() << functionName << message << "\n";
}

std::string Logger::getTimestamp()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - Utilities::getStartTime());

    double elapsedSeconds = elapsed.count() / 1000.0;

    std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now();

    return std::format("[{:%H:%M:%S}] [{:.6f}s] ", systemNow, elapsedSeconds);
}
