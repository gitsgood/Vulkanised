#ifndef UTILITIES_H
#define UTILITIES_H

#include <vulkan/vulkan.h>
#include <vector>
#include <chrono>

namespace Utilities
{
    // using inline so header-only definition doesn't produce duplicate symbols when included from multiple translation units (TUs).
    inline const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
    inline bool enableValidationLayers = false;
#else
    inline bool enableValidationLayers = true;
#endif

    // Make these inline to avoid multiple-definition linker errors when included from multiple TUs.
    inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    // Indices (locations) of queue families (if they exist at all)
    struct QueueFamilyIndices
    {
        int graphicsFamily = -1;	// Location of the graphics queue family

        // Check if queue families are valid
        bool isValid() const
        {
            return graphicsFamily >= 0;
        }
    };

    // Path constants. The definitions are set up in the CMakeLists file. It's where the logic on how the compiler knows lies.
#if defined(BUILD_ENV_VISUAL_STUDIO)
    inline constexpr const char* PATH = "../../../";
#elif defined(BUILD_ENV_QTCREATOR)
    inline constexpr const char* PATH = "../../";
#else
    inline constexpr const char* PATH = "../../"; // fallback e.g. Mac
#endif

    // Static local time. We can use this to log order of operations, as well as their speed. Initialised at the first line of main and stays that way for the rest of runtime.
    inline std::chrono::steady_clock::time_point getStartTime() {
        static std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        return startTime;
    }
}

#endif // !UTILITIES_H
