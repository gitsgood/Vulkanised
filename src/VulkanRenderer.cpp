#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	if (newWindow == nullptr) {
		printf("ERROR: GLFW window is null\n");
		return EXIT_FAILURE;
	}

	// Wrap it in a shared_ptr with a custom deleter
	// We pass glfwDestroyWindow as the function to call when the ref count hits 0
	m_Window = std::shared_ptr<GLFWwindow>(newWindow, glfwDestroyWindow);

	try
	{
		createInstance();
		getPhysicalDevice();
		createLogicalDevice();
	}
	catch (const std::runtime_error &e)
	{
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void VulkanRenderer::cleanup()
{
	if (m_MainDevice.logicalDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_MainDevice.logicalDevice, nullptr);
		m_MainDevice.logicalDevice = VK_NULL_HANDLE;
	}
	if (m_Instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_Instance, nullptr);
		m_Instance = VK_NULL_HANDLE;
	}
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{
	// Application information
	// Most data here doesn't affect the program, it's for dev convenience.
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkanised";						// Custom application name
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);			// Custom application version
	appInfo.pEngineName = "No Engine... yet";						// Custom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);				// Custom engine version
	appInfo.apiVersion = VK_API_VERSION_1_4;						// Vulkan API version

	// Creation information for a Vulkan instance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	// Set up extensions Instance will use
	uint32_t glfwExtensionCount{ 0 };								// GLFW may require multiple extensions
	const char** glfwExtensions;									// Extensions passed as an array of cstrings, so we need pointer (the array) to pointer (the cstring)

	// Get GLFW extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add GLFW extensions to our list
	instanceExtensions.reserve(glfwExtensionCount);
	for (size_t i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	// Check if instance extensions are supported...
	if(!checkInstanceExtensionSupport(&instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions!");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensionCount);
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// TODO: Set up validations layers that instance will use.
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	// Create instance
	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vulkan Instance!");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	// Get the queue family indices for the chosen Physical device
	QueueFamilyIndices indices = getQueueFamilies(m_MainDevice.physicalDevice);

	// Queue the logical device needs to create and info to do so (only 1 for now, will add more later!)
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;				// Index of the family to create a queue from
	queueCreateInfo.queueCount = 1;											// Number of queues to create
	float priority{ 1.f };
	queueCreateInfo.pQueuePriorities = &priority;							// Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)


	// Information to create logical device (sometimes called "device")
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;							// Number of Queue Create infos
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;				// List of queue create infos so device can create required queues
	deviceCreateInfo.enabledExtensionCount = 0;							// Number of enabled logical device extensions
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;					// List of enabled logical device extensions

	// Physical Device Features the logical device will be using
	VkPhysicalDeviceFeatures deviceFeatures{};

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;				// Physical Device features Logical Device will use

	// Create the logical device for the given physical device
	if (vkCreateDevice(m_MainDevice.physicalDevice, &deviceCreateInfo, nullptr, &m_MainDevice.logicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Logical Device!");
	}

	// Queues are created at the same time as the device...
	// So we want handle to queues
	// For given Logical Device, of given Queue Family, of given Queue Index (0 since only one queue), place reference in given VkQueue
	vkGetDeviceQueue(m_MainDevice.logicalDevice, indices.graphicsFamily, 0, &m_GraphicsQueue);
}

void VulkanRenderer::getPhysicalDevice()
{
	// Enumerate physical devices the VkInstance can access
	uint32_t deviceCount{ 0 };
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	// If no devices available, then none support Vulkan!
	if (deviceCount == 0)
	{
		throw std::runtime_error("Can't find GPU's that support Vulkan instance!");
	}

	// Get list of physical devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, deviceList.data());

	for (const auto& device : deviceList)
	{
		if (checkDeviceSuitable(device))
		{
			m_MainDevice.physicalDevice = device;
			break;
		}
	}

}

bool VulkanRenderer::checkInstanceExtensionSupport(const std::vector<const char*> *checkExtensions)
{
	// Need to get number of extensions to create array of correct size to hold extensions
	uint32_t extensionCount{ 0 };
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create a list of vkExtensionProperties using count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Check if given extensions are in list of available extensions
	for (const auto &checkExtension : *checkExtensions)
	{
		bool hasExtension{ false };
		for(const auto &extension : extensions)
		{
			if (strcmp(checkExtension, extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if(!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	//// Information about the device itself (ID, name, type, vendor, etc)
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//// Information about what the device can do (geo shader, tess shader, wide lines, etc)
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = getQueueFamilies(device);

	return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all queue family property info for the given device
	uint32_t queueFamilyCount{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Go through each queue family and check if it has at least 1 of the required types of queue
	int i{ 0 };
	for (const auto &queueFamily : queueFamilyList)
	{
		// First check if queue family has at least 1 queue in that family (could have no queues)
		// Queue can be multiples types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to check if has required type
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;		// If queue family is valid, then get index

		}

		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid())
		{
			break;
		}

		i++;
	}

	return indices;
}
