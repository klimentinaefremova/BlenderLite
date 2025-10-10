cmake_minimum_required(VERSION 3.0)
project(OpenGLPrj)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

# Build GLFW from vendor
add_subdirectory(vendor/glfw)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

# Include directories (keep your other vendor includes)
include_directories(
        include/
        vendor/glad/include/
        vendor/glfw/include/
        vendor/glm/
        vendor/stb/
        vendor/freeglut/include/         # <-- freeglut headers
)

# Vendors sources (glad)
file(GLOB VENDORS_SOURCES vendor/glad/src/glad.c)

# Project files
file(GLOB PROJECT_HEADERS include/*.hpp)
file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_SHADERS shaders/*.comp shaders/*.frag shaders/*.geom shaders/*.vert shaders/*.vs shaders/*.fs )
file(GLOB PROJECT_CONFIGS CMakeLists.txt Readme.md .gitattributes .gitignore .gitmodules)

source_group("include" FILES ${PROJECT_HEADERS})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("vendors" FILES ${VENDORS_SOURCES})

# Definitions you had
add_definitions(-DGLFW_INCLUDE_NONE -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

# Create executable
add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_SHADERS}
        ${PROJECT_CONFIGS}
        ${VENDORS_SOURCES}
)

# Path to your freeglut import library and DLL (adjust arch folder if needed)
set(FREEGLUT_LIB "${CMAKE_SOURCE_DIR}/vendor/freeglut/lib/x64/freeglut.lib")
set(FREEGLUT_DLL "${CMAKE_SOURCE_DIR}/vendor/freeglut/bin/x64/freeglut.dll")

# Link libraries (keep order: glfw, glfw libs, glad, freeglut)
target_link_libraries(${PROJECT_NAME}
        glfw
        ${GLFW_LIBRARIES}
        ${GLAD_LIBRARIES}
        "${FREEGLUT_LIB}"      # link freeglut import library
)

# On Windows link opengl32 (required)
if (WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()

# Make sure freeglut DLL is copied next to the executable after build so it runs
if(EXISTS "${FREEGLUT_DLL}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FREEGLUT_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
else()
    message(WARNING "freeglut DLL not found at ${FREEGLUT_DLL}. If you run into runtime errors, check that the DLL exists and path is correct.")
endif()

# Set runtime output directory (keeps your original behavior)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
