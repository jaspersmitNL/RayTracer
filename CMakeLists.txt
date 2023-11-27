cmake_minimum_required(VERSION 3.26)
project(RayTracer)

set(CMAKE_CXX_STANDARD 20)
find_package(glm CONFIG REQUIRED)

add_executable(RayTracer src/main.cpp src/vendor/stb_image_write.h src/vendor/BS_thread_pool.hpp
        src/ray.hpp
        src/sphere.hpp
        src/object.hpp
        src/scene.hpp
        src/material.hpp)


target_link_libraries(RayTracer PRIVATE glm::glm)
