#ifndef FILE_UTILS_H
#define FILE_UTILS_H

namespace Vulkanised
{
	namespace Utilities
	{
		namespace File
		{
			// Path constants. The definitions are set up in the CMakeLists file. It's where the logic on how the compiler knows lies.
			#if defined(BUILD_ENV_VISUAL_STUDIO)
			inline constexpr const char* c_Path{ "../../../" };
			#elif defined(BUILD_ENV_QTCREATOR)
			inline constexpr const char* c_Path{ "../../" };
			#else
			inline constexpr const char* c_Path{ "../../" }; // fallback e.g. Mac
			#endif
			inline constexpr const char* c_LogSettingsFileName{ "VulkanisedLoggerSettings.json" };
			inline constexpr const char* c_LogFileName{ "VulkanisedLog.log" };

			inline constexpr const char* c_LogSeparation{ "\n------------------------------------------------------------\n\n" };
		}
	}
}

#endif // !FILE_UTILS_H
