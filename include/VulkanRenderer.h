#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <atomic>
#include <set>
#include <algorithm>

#include "Utilities.h"
#include "Logger.h"
#include "Vertex.h"
#include "Mesh.h"

namespace Vulkanised
{
class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);

	inline void updateModel(int modelId, glm::mat4 newModel) noexcept { if (modelId >= m_MeshList.size()) return; m_MeshList[modelId]->setModel(newModel); }

	void draw();
	void cleanup() noexcept;

	~VulkanRenderer();

private:
	// GLFW Window
	VAtomic<GLFWwindow> m_Window;

	int m_CurrentFrame{ 0 };

	// Scene objects
	std::vector<std::unique_ptr<Mesh>> m_MeshList;

	// Scene settings
	struct UboViewProjection {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
	} m_UboViewProjection;

	// Vulkan Components
	// - Main
	VkInstance m_Instance;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} m_MainDevice;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentationQueue;
	VkDebugUtilsMessengerEXT m_DebugMessenger;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_Swapchain;
	std::vector<Utilities::Vulkan::SwapchainImage> m_SwapchainImages;
	std::vector<VkFramebuffer> m_SwapchainFramebuffers;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	VkImage m_DepthBufferImage;
	VkDeviceMemory m_DepthBufferImageMemory;
	VkImageView m_DepthBufferImageView;

	// - Descriptors
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkPushConstantRange m_PushConstantRange;

	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	//std::vector<VkBuffer> m_UniformBuffer;
	//std::vector<VkDeviceMemory> m_UniformBufferMemory;

	std::vector<VkBuffer> m_VpUniformBuffer;
	std::vector<VkDeviceMemory> m_VpUniformBufferMemory;

	std::vector<VkBuffer> m_ModelDUniformBuffer;
	std::vector<VkDeviceMemory> m_ModelDUniformBufferMemory;

	//VkDeviceSize m_MinUniformBufferOffset;
	//size_t m_ModelUniformAlignment;
	//UboModel* m_ModelTransferSpace;

	// - Pipeline
	VkPipeline m_GraphicsPipeline;
	VkPipelineLayout m_PipelineLayout;
	VkRenderPass m_RenderPass;

	// - Pools
	VkCommandPool m_GraphicsCommandPool;

	// - Utility
	VkFormat m_SwapchainImageFormat;
	VkExtent2D m_SwapchainExtent;

	// - Synchronisation
	std::vector<VkSemaphore> m_ImageAvailable;
	std::vector<VkSemaphore> m_RenderFinished;
	std::vector<VkFence> m_DrawFences;

	// Validation layer trackers
	std::uint16_t m_PerformanceMsgCount{ 0 };	// Validation layer detects that we can probably do better for performance
	std::uint16_t m_ValidationMsgCount{ 0 };	// Means we broke a Vulkan rule, we ought to look into it
	std::uint16_t m_ErrorMsgCount{ 0 };			// We probably fucked up, ideally this stays at 0...
	std::uint16_t m_WarningMsgCount{ 0 };		// Amount of things we should look into;

	// - Validation tracker printers
	void logTrackers() const noexcept;

	// Vulkan Functions
	// - Create functions
	void createInstance();
	void createLogicalDevice();
	void setupDebugMessenger();
	void createSurface();
	void createSwapchain();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createPushConstantRange();
	void createGraphicsPipeline();
	void createDepthBufferImage();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronisation();

	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();

	void updateUniformBuffers(uint32_t imageIndex);			// Called in draw()

	// - Record functions
	void recordCommands(uint32_t currentImage);

	// - Get functions
	void getPhysicalDevice();

	// - Allocate Functions
	void allocateDynamicBufferTransferSpace();

	// - Support functions
	// -- Checker functions
	bool checkInstanceExtensionSupport(const std::vector<const char*>* checkExtensions);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool checkValidationLayerSupport();
	bool checkDeviceSuitable(VkPhysicalDevice device);


	// -- Getter functions
	Utilities::Vulkan::QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	Utilities::Vulkan::SwapchainDetails getSwapchainDetails(VkPhysicalDevice device);

	// -- Choose functions
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);
	VkFormat chooseSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) const;

	// -- Create functions
	VkImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags, VkDeviceMemory* imageMemory) const;
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;
	VkShaderModule createShaderModule(const std::vector<char>& code) const;
};
}

#endif // !VULKANRENDERER_H
