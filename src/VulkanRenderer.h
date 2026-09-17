#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <atomic>
#include <set>

#include "Utilities.h"
#include "Logger.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void cleanup();

	~VulkanRenderer();

private:
	// GLFW Window
	std::atomic<std::shared_ptr<GLFWwindow>> m_Window;

	// Vulkan Components
	VkInstance m_Instance;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} m_MainDevice;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentationQueue;
	VkDebugUtilsMessengerEXT m_DebugMessenger;
	VkSurfaceKHR m_Surface;

	// Vulkan Functions
	// - Create functions
	void createInstance();
	void createLogicalDevice();
	void setupDebugMessenger();
	void createSurface();

	// - Get functions
	void getPhysicalDevice();

	// - Support functions
	// -- Checker functions
	bool checkInstanceExtensionSupport(const std::vector<const char*> *checkExtensions);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool checkValidationLayerSupport();
	bool checkDeviceSuitable(VkPhysicalDevice device);
	

	// -- Getter functions
	Utilities::QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);
	Utilities::SwapchainDetails getSwapchainDetails(VkPhysicalDevice device);
};

#endif // !VULKANRENDERER_H
