cmake_minimum_required(VERSION 3.20)
project(BlenderLite)

set(CMAKE_CXX_STANDARD 17)

# Build GLFW
add_subdirectory(vendor/glfw)

# Include directories
include_directories(
        include
        vendor/glad/include
        vendor/glfw/include
        vendor/glm
        vendor/stb
        vendor/freeglut/include
)

# Source files - use GLOB_RECURSE to find all .cpp files
file(GLOB_RECURSE SOURCES
        src/*.cpp
        vendor/glad/src/glad.c
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Find OpenGL and GLU
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        glfw
        ${GLFW_LIBRARIES}
        "${CMAKE_SOURCE_DIR}/vendor/freeglut/lib/x64/freeglut.lib"
        OpenGL::GL
        glu32 # Add GLU library
)

# Copy DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/freeglut/bin/x64/freeglut.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# Copy textures directory to build folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/textures"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
)