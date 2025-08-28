cmake_minimum_required(VERSION "3.19.2")


include_directories(
    "${CMAKE_SOURCE_DIR}/external/include/"
    "${CMAKE_SOURCE_DIR}/Source/ImGui/"
    "${CMAKE_SOURCE_DIR}/Source/Utilities/CommonUtilities/include"
    "${CMAKE_SOURCE_DIR}/external/include/physx/"
    "${CMAKE_SOURCE_DIR}/Source/Engine/GraphicsEngine/"
    "${CMAKE_SOURCE_DIR}/Source/Engine/GameEngine/"
    "${CMAKE_SOURCE_DIR}/Source/Application/"
    "${CMAKE_SOURCE_DIR}/Source/"
    "${CMAKE_SOURCE_DIR}/Source/Networking/"
)

project(File_Collection LANGUAGES CXX)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS 
"${CMAKE_SOURCE_DIR}/Source/*.cpp"
"${CMAKE_SOURCE_DIR}/Source/*.h"
"${CMAKE_SOURCE_DIR}/Source/*.hpp"
"${CMAKE_SOURCE_DIR}/Source/*."
)

add_executable(File_Collection ${ALL_SOURCES})