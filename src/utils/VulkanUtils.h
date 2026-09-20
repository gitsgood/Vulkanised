#ifndef VULKANUTILS_H
#define VULKANUTILS_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkanised
{
    namespace Utilities
    {
        namespace Vulkan
        {
            inline constexpr int c_MaxFrameDraws{ 2 };

            // using inline so header-only definition doesn't produce duplicate symbols when included from multiple translation units (TUs).
            inline const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

            #ifdef NDEBUG
            inline bool enableValidationLayers = false;
            #else
            inline bool enableValidationLayers = true;
            #endif

            // Make these inline to avoid multiple-definition linker errors when included from multiple TUs.
            inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
            {
                auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
                if (func != nullptr) {
                    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
                }
                else {
                    return VK_ERROR_EXTENSION_NOT_PRESENT;
                }
            }

            inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
            {
                auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
                if (func != nullptr) {
                    func(instance, debugMessenger, pAllocator);
                }
            }

            inline const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            // Indices (locations) of queue families (if they exist at all)
            struct QueueFamilyIndices
            {
                int graphicsFamily{ -1 };	    // Location of the graphics queue family
                int presentationFamily{ -1 };   // Location of presentation queue family

                // Check if queue families are valid
                bool isValid() const
                {
                    return graphicsFamily >= 0 && presentationFamily >= 0;
                }
            };

            struct SwapchainDetails
            {
                VkSurfaceCapabilitiesKHR surfaceCapabilities;       // Surface properties, e.g. image, size/extent
                std::vector<VkSurfaceFormatKHR> formats;            // Surface image formats, e.g. RBGA and size of each color
                std::vector<VkPresentModeKHR> presentationModes;    // How images should be presented to screen
            };

            struct SwapchainImage
            {
                VkImage image;
                VkImageView imageView;
            };
        }
    }
}

#endif // !VULKANUTILS_H
