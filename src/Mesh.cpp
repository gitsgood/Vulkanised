#include "Mesh.h"

Vulkanised::Mesh::Mesh()
{
}

Vulkanised::Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices) 
	: m_PhysicalDevice(newPhysicalDevice), m_LogicalDevice(newDevice), m_VertexCount(vertices->size())
{
	LOG("A new mesh joins the ranks...");
	createVertexBuffer(vertices);
}

size_t Vulkanised::Mesh::getVertexCount() const noexcept
{
	return m_VertexCount;
}

VkBuffer Vulkanised::Mesh::getVertexBuffer() const noexcept
{
	return m_VertexBuffer;
}

Vulkanised::Mesh::~Mesh()
{
	LOG("A mesh has begun its destruction!");

	if (m_VertexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(m_LogicalDevice, m_VertexBuffer, nullptr);
		m_VertexBuffer = VK_NULL_HANDLE;
	}
	if (m_VertexBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(m_LogicalDevice, m_VertexBufferMemory, nullptr);
		m_VertexBufferMemory = VK_NULL_HANDLE;
	}
}

void Vulkanised::Mesh::createVertexBuffer(std::vector<Vertex>* vertices)
{
	// CREATE VERTEX BUFFER
	// Information to create a buffer (doesn't include assigning memory)
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(Vertex) * vertices->size();			// Size of buffer (size of 1 vertex * number of vertices)
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;			// Multiple types of buffer possible, we want Vertex buffer
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;				// Similar to swapchain images, can share vertex buffers

	if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create a vertex buffer");
	}

	// GET BUFFER MEMORY REQUIREMENTS
	VkMemoryRequirements memRequirements{};
	vkGetBufferMemoryRequirements(m_LogicalDevice, m_VertexBuffer, &memRequirements);

	// ALLOCATE MEMORY TO BUFFER
	VkMemoryAllocateInfo memoryAllocInfo{};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memRequirements.size;
	memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,		// Index of memory type on Physical device that has required bit flags
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);			// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	: CPU can interact with memory
																								// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT	: Allows placement of data straight into buffer after mapping (otherwise would have to specify manually)

	// Allocate memory to VkDeviceMemory
	if (vkAllocateMemory(m_LogicalDevice, &memoryAllocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory");
	}

	// Allocate memory to given vertex buffer
	if (vkBindBufferMemory(m_LogicalDevice, m_VertexBuffer, m_VertexBufferMemory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to bind buffer memory!");
	}

	// MAP MEMORY TO VERTEX BUFFER
	void* data;																				// 1. Create pointer to a point in normal memory
	vkMapMemory(m_LogicalDevice, m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);		// 2. "Map" the vertex buffer memory to that point
	memcpy(data, vertices->data(), (size_t)bufferInfo.size);								// 3. Copy memory from vertices vector to the point
	vkUnmapMemory(m_LogicalDevice, m_VertexBufferMemory);									// 4. Unmap the vertex buffer memory
}

uint32_t Vulkanised::Mesh::findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties) const noexcept
{
	// Get properties of physical device memory
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

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
