cmake_minimum_required(VERSION "3.19.2")

project(ImGui LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(ImGui STATIC)

file(GLOB_RECURSE ImGui_LIB_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGui/*.cpp" 
)

target_link_libraries(ImGui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}../../../external/lib/glfw3.lib")
target_link_libraries(ImGui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}../../../external/lib/glfw3_mt.lib")

target_sources(ImGui PRIVATE ${ImGui_LIB_SOURCES})

target_include_directories(ImGui PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGui/"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../external/include/"
)

add_definitions(-D_DEBUG)


set_target_properties(ImGui PROPERTIES 
    OUTPUT_NAME_DEBUG "ImGui"
    OUTPUT_NAME_RELEASE "ImGui"
)