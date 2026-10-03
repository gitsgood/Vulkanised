#ifndef VULKANUTILS_H
#define VULKANUTILS_H

#include <vulkan/vulkan.h>
#include <vector>

#include "Logger.h"

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
                [[nodiscard]] bool isValid() const noexcept
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

            [[nodiscard]] inline uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes, VkMemoryPropertyFlags properties) noexcept
            {
                // Get properties of physical device memory
                VkPhysicalDeviceMemoryProperties memoryProperties;
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

                uint32_t retVal{ 0 };
                for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
                {
                    if ((allowedTypes & (1 << i))															// Index of memory type must correspond bit in allowedTypes
                        && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)		// The desired property bit flags are part of memory type's property flag 
                    {
                        // This memory type is valid, so return its index
                        retVal = i;
                        return i;
                    }
                }
                return retVal;
            }

            inline void createBuffer(
                VkPhysicalDevice physicalDevice, 
                VkDevice logicalDevice, 
                VkDeviceSize bufferSize, 
                VkBufferUsageFlags bufferUsage, 
                VkMemoryPropertyFlags bufferProperties, 
                VkBuffer* buffer, 
                VkDeviceMemory* bufferMemory)
            {
                // CREATE ~~VERTEX~~ ANY BUFFER
                // Information to create a buffer (doesn't include assigning memory)
                VkBufferCreateInfo bufferInfo{};
                bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size = bufferSize;			                        // Size of buffer (size of 1 vertex * number of vertices)
                bufferInfo.usage = bufferUsage;			                        // Multiple types of buffer possible, ~~we want Vertex buffer~~ 
                bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;				// Similar to swapchain images, can share vertex buffers

                if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to create a buffer" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }

                // GET BUFFER MEMORY REQUIREMENTS
                VkMemoryRequirements memRequirements{};
                vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memRequirements);

                // ALLOCATE MEMORY TO BUFFER
                VkMemoryAllocateInfo memoryAllocInfo{};
                memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memoryAllocInfo.allocationSize = memRequirements.size;
                memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits,		// Index of memory type on Physical device that has required bit flags
                    bufferProperties);			                                                            // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	: CPU can interact with memory
                                                                                                            // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	: Allows placement of data straight into buffer after mapping (otherwise would have to specify manually)

                // Allocate memory to VkDeviceMemory
                if (vkAllocateMemory(logicalDevice, &memoryAllocInfo, nullptr, bufferMemory) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to allocate buffer memory" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }

                // Allocate memory to given vertex buffer
                if (vkBindBufferMemory(logicalDevice, *buffer, *bufferMemory, 0) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to bind buffer memory!" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }
            }

            inline void copyBuffer(
                VkDevice logicalDevice, 
                VkQueue transferQueue, 
                VkCommandPool transferCommandPool, 
                VkBuffer srcBuffer,
                VkBuffer dstBuffer,
                VkDeviceSize bufferSize)
            {
                // Command buffer to hold transfer commands
                VkCommandBuffer transferCommandBuffer;

                // Command buffer details
                VkCommandBufferAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocInfo.commandPool = transferCommandPool;
                allocInfo.commandBufferCount = 1;

                // Allocate command buffer from pool
                if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &transferCommandBuffer) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to allocate command buffer" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }

                // Information to begin a command buffer record
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;  // We're only using the command buffer once, so set up for one time submit

                // Begin recording transfer commands
                if (vkBeginCommandBuffer(transferCommandBuffer, &beginInfo) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to record transfer commands" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }

                // Region of data to copy from and to
                VkBufferCopy bufferCopyRegion{};
                bufferCopyRegion.srcOffset = 0;
                bufferCopyRegion.dstOffset = 0;
                bufferCopyRegion.size = bufferSize;

                // Command to copy src buffer to dst buffer
                vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);

                // End commands
                if (vkEndCommandBuffer(transferCommandBuffer) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to end the transfer command buffer" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }

                // Queue submission information
                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &transferCommandBuffer;

                // This assume a small amount of things to render
                // TODO: Look into optimising this

                // Submit transfer commands to transfer queue and wait until it finishes
                if (vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
                {
                    constexpr const char* message{ "failed to submit command to transfer queue" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }
                if (vkQueueWaitIdle(transferQueue) != VK_SUCCESS)
                {
                    constexpr const char* message{ "somehow failed to wait until transfer queue finishes whatever its doing" };
                    LOGE(message);
                    throw std::runtime_error(message);
                }

                // Free temporary command buffer back to pool
                vkFreeCommandBuffers(logicalDevice, transferCommandPool, 1, &transferCommandBuffer);
            }
        }
    }
}

#endif // !VULKANUTILS_H
