cmake_minimum_required(VERSION 3.31)
project(Vulkanised)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force GLFW not to build its own tests, examples, or docs
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Tell CMake to look inside the GLFW folder
add_subdirectory(external/glfw-3.5.1)

# Glaze is a fast json library for cpp. It requires c++ 23
add_subdirectory(external/glaze)

# Ask CMake to find the Vulkan SDK installed on the computer
find_package(Vulkan REQUIRED)

# Detect build environment based on CMAKE_BINARY_DIR
# Used to set correct PATH and some clean up
if(${CMAKE_BINARY_DIR} MATCHES ".*out/.*")
    add_compile_definitions(BUILD_ENV_VISUAL_STUDIO)
    message(STATUS "=== Visual Studio detected ===")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build")
        file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/build")
    endif()
elseif(${CMAKE_BINARY_DIR} MATCHES ".*build/.*")
    add_compile_definitions(BUILD_ENV_QTCREATOR)
    message(STATUS "=== Qt Creator detected ===")
    #Deleting preset files not used in Qt - generated in VS 2022
    #This should not fail even if the files are not present
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/CMakePresets.json")
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/CMakeUserPresets.json")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/out")
        file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/out")
    endif()
endif()

# Set source files...
set(SOURCES
    src/main.cpp
    src/Utilities.h
    src/utils/VulkanUtils.h src/utils/TimeUtils.h   src/utils/FileUtils.h   src/utils/CrossPlatUtils.h
    src/Logger.h            src/Logger.cpp
    src/VulkanRenderer.h    src/VulkanRenderer.cpp
)

# all shader and script files shown in IDE
file(GLOB SHADER_FILES
    "src/shaders/*.frag"
    "src/shaders/*.vert"
)

# Define your executable using the SOURCES variable
add_executable(${PROJECT_NAME} ${SOURCES} ${SHADER_FILES})

# Tell CMake this specific target requires C++23
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Explicitly set the subsystem to CONSOLE for Windows, if it's on DEBUG config.
if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
        #$<$<CONFIG:Release>:/SUBSYSTEM:CONSOLE>    # For comparing debug and release speed with console output...
    )
endif()

# Link the libraries
target_link_libraries(${PROJECT_NAME} glfw Vulkan::Vulkan glaze::glaze)

# Shader compilation, hopefully cross-platform
if(APPLE)
    add_custom_target(CompileShaders
        COMMENT "Compiling Vulkan shaders for macOS"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
        COMMAND bash CompileShadersMac.sh   # Will need to double check what CompileShadersMac.sh does and adapt it
    )
elseif(MSVC)
    add_custom_target(CompileShaders
        COMMENT "Compiling Vulkan shaders for Windows"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
        COMMAND "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe" -V "shader.vert" -o "vert.spv"
        COMMAND "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe" -V "shader.frag" -o "frag.spv"
    )
endif()

add_dependencies(${PROJECT_NAME} CompileShaders)