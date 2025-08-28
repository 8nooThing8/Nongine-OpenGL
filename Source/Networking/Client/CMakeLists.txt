cmake_minimum_required(VERSION "3.19.2")

project(Client LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(Client STATIC)

file(GLOB_RECURSE CU_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(Client PRIVATE ${CU_LIB_SOURCES} )

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Utilities/CommonUtilities/include"
    "${CMAKE_SOURCE_DIR}/external/include"
)

target_compile_definitions(Client PRIVATE OPENGL)
    if (EMSCRIPTEN)
        target_compile_definitions(Client PRIVATE BOOST_ASIO_DISABLE_SIGNAL_BLOCKING)
    endif(EMSCRIPTEN)

add_definitions(-D_DEBUG)

target_link_libraries(Client PRIVATE CommonUtilities)

set_target_properties(Client PROPERTIES 
    OUTPUT_NAME_DEBUG "Client"
    OUTPUT_NAME_RELEASE "Client"
)