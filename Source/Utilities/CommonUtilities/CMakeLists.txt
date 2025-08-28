cmake_minimum_required(VERSION "3.19.2")

project(CommonUtilities LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(CommonUtilities STATIC)

file(GLOB_RECURSE CU_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(CommonUtilities PRIVATE ${CU_LIB_SOURCES} )

target_compile_definitions(CommonUtilities PRIVATE OPENGL)

add_definitions(-D_DEBUG)

target_include_directories(CommonUtilities PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(CommonUtilities PROPERTIES 
    OUTPUT_NAME_DEBUG "CommonUtilities"
    OUTPUT_NAME_RELEASE "CommonUtilities"
)
