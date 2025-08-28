cmake_minimum_required(VERSION "3.19.2")

project(Server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(Server STATIC)

file(GLOB_RECURSE CU_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(Server PRIVATE ${CU_LIB_SOURCES} )

include_directories(
    "${CMAKE_SOURCE_DIR}/Source/Utilities/CommonUtilities/include"
    "${CMAKE_SOURCE_DIR}/external/include"
    "${CMAKE_SOURCE_DIR}/Source/"
    "${CMAKE_SOURCE_DIR}/Source/Networking/"
)

target_compile_definitions(Server PRIVATE OPENGL)
    if (EMSCRIPTEN)
        target_compile_definitions(Server PRIVATE BOOST_ASIO_DISABLE_SIGNAL_BLOCKING)
    endif(EMSCRIPTEN)

add_definitions(-D_DEBUG)

target_link_libraries(Server PRIVATE CommonUtilities)

set_target_properties(Server PROPERTIES 
    OUTPUT_NAME_DEBUG "Server"
    OUTPUT_NAME_RELEASE "Server"
)