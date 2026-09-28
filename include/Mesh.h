#ifndef MESH_H
#define MESH_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>

#include "Vertex.h"
#include "Logger.h"

namespace Vulkanised
{

class Mesh
{
public:
	Mesh();
	Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices);

	size_t getVertexCount() const noexcept;
	VkBuffer getVertexBuffer() const noexcept;

	~Mesh();

	// C++ over here making shallow fucking copies of my meshes ffs, at least now someone trying to just assign a mesh with "=" will not even compile
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
private:
	size_t m_VertexCount{ 0 };
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;

	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_LogicalDevice;
	
	void createVertexBuffer(std::vector<Vertex>* vertices);
	uint32_t findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties) const noexcept;
};

}

#endif // !MESH_H
