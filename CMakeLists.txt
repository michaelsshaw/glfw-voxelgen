cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(project01)

include_directories(include)
include_directories(lib/cglm/include lib/glad/include lib/glfw/include)


file(GLOB_RECURSE SRCS src/*.c)
file(COPY res/shaders DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY res/img DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(minecraft ${SRCS} lib/glad/src/glad.c)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(minecraft m)
if(NOT WIN32)
    target_link_libraries(minecraft dl)
endif(NOT WIN32)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})

target_link_libraries(minecraft ${OPENGL_LIBRARIES})

add_subdirectory(lib/cglm)
add_subdirectory(lib/glfw)
target_link_libraries(minecraft glfw ${GLFW_LIBRARIES} cglm ${CGLM_LIBRARIES})
