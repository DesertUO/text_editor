cmake_minimum_required(VERSION 3.26)
project(TextEditor CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/source)

add_executable(TEditor WIN32 main.cpp)

target_sources(TEditor
PRIVATE
    main.cpp
    utils.h utils.cpp
    mainEditor.h mainEditor.cpp
    TextBuffer.h TextBuffer.cpp
)
target_include_directories(TEditor PRIVATE ${SDL3_INCLUDE_DIRS})
target_link_libraries(TEditor PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)


# Copy textures folder to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/textures DESTINATION ${CMAKE_BINARY_DIR}/src)
file(COPY ${CMAKE_SOURCE_DIR}/src/fonts DESTINATION ${CMAKE_BINARY_DIR}/src)

