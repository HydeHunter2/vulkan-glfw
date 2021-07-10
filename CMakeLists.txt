cmake_minimum_required(VERSION 3.17)
project(vulkan)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan)

add_executable(vulkan application.cpp main.cpp)

target_link_libraries(vulkan glfw)
target_link_libraries(vulkan Vulkan::Vulkan)
