#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include <atomic>
#include <string_view>

#include "VulkanRenderer.h"

namespace Vulkanised
{
	// Little trick I stole from Rust. Ideally this should be immutable and thread-safe, but at least it is supposed to be somehow thread-safe now. This might come in handy down the line.
	std::atomic<std::shared_ptr<GLFWwindow>> window;
	VulkanRenderer renderer;

	static void initWindow(std::string_view wName = "Test Window", const int width = 800, const int height = 600)
	{
		if (glfwInit() == GLFW_FALSE)
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}

		// Set GLFW to not create an OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// Create the raw pointer
		GLFWwindow* rawWindow = glfwCreateWindow(width, height, wName.data(), nullptr, nullptr);

		if (!rawWindow) {
			throw std::runtime_error("Failed to create GLFW window");
		}

		// Wrap it in a shared_ptr with a custom deleter
		// We pass glfwDestroyWindow as the function to call when the ref count hits 0
		window = std::shared_ptr<GLFWwindow>(rawWindow, glfwDestroyWindow);
	}
}

int main()
{
	using namespace Vulkanised;

	// Set the initial timestamp before anything
	Utilities::Time::getStartTime();
	//Utilities::Time::timeIt("getStartTime", Utilities::Time::getStartTime);

	// Initialises the logger (necessary, since it uses lazy initialisation), while also "announcing" the initialisation of the program.
	LOGH("Vulkanised hath started!");
	//Utilities::Time::timeIt("Initialising logger and logging", [&]() { Logger::getLoggerInstance()->logText(Logger::LogType::HIGHLIGHT, std::source_location::current(), "Vulkanised hath started!"); });

	// Create window
	initWindow("Test Window", 800, 600);

	// Create Vulkan renderer instance and initialize it with the window
	if (renderer.init(window.load().get()) == EXIT_FAILURE)
	{
		printf("Failed to initialize Vulkan renderer\n");
		return EXIT_FAILURE;
	}

	// Loop until closed
	while (!glfwWindowShouldClose(window.load().get()))
	{
		glfwPollEvents();
	}

	renderer.cleanup();

	return EXIT_SUCCESS;
}