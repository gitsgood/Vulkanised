#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <memory>
#include <atomic>

#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void cleanup();

	~VulkanRenderer();

private:
	std::atomic<std::shared_ptr<GLFWwindow>> m_Window;

	// Vulkan Components
	VkInstance m_Instance;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} m_MainDevice;
	VkQueue m_GraphicsQueue;

	// Vulkan Functions
	// - Create functions
	void createInstance();
	void createLogicalDevice();

	// - Get functions
	void getPhysicalDevice();

	// - Support functions
	// -- Checker functions
	bool checkInstanceExtensionSupport(const std::vector<const char*> *checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- Getter functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};

#endif // !VULKANRENDERER_H
