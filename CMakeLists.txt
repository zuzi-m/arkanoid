cmake_minimum_required(VERSION 3.16)
project(Arkanoid LANGUAGES CXX)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    src/main.cpp
    src/Game.cpp
    src/GeometryEngine.cpp
    src/LevelController.cpp
    src/LevelFactory.cpp
    src/Renderer.cpp
)

add_executable(Arkanoid ${SOURCE_FILES})
target_include_directories(Arkanoid PRIVATE src)

# Link to the actual SDL3 library.
target_link_libraries(Arkanoid PRIVATE SDL3::SDL3)