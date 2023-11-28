cmake_minimum_required(VERSION 3.26)
project(RayTracer)

set(CMAKE_CXX_STANDARD 20)
find_package(glm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# enable speed optimizations for debug builds

file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp src/*.h)


add_executable(RayTracer ${SOURCES})


target_link_libraries(RayTracer
        PRIVATE
        glm::glm
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)