#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <atomic>

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
	VkDebugUtilsMessengerEXT m_DebugMessenger;

	// Vulkan Functions
	// - Create functions
	void createInstance();
	void createLogicalDevice();
	void setupDebugMessenger();

	// - Get functions
	void getPhysicalDevice();

	// - Support functions
	// -- Checker functions
	bool checkInstanceExtensionSupport(const std::vector<const char*> *checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);
	bool checkValidationLayerSupport();

	// -- Getter functions
	Utilities::QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);
};

#endif // !VULKANRENDERER_H
