cmake_minimum_required(VERSION 3.19)
project(Vulkanised)

# Force GLFW not to build its own tests, examples, or docs
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Tell CMake to look inside the GLFW folder
add_subdirectory(external/glfw-3.5.1)

# Ask CMake to find the Vulkan SDK installed on the computer
find_package(Vulkan REQUIRED)

# Set source files...
set(SOURCES
    src/main.cpp
    src/Utilities.h
    src/VulkanRenderer.h    src/VulkanRenderer.cpp
)

# all shader and script files shown in IDE
# file(GLOB SHADER_FILES)

# Define your executable using the SOURCES variable
add_executable(${PROJECT_NAME} ${SOURCES})

# Tell CMake this specific target requires C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Explicitly set the subsystem to CONSOLE for Windows, if it's on DEBUG config.
if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    )
endif()

# Link the libraries
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan)