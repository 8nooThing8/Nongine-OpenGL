cmake_minimum_required(VERSION "3.19.2")

project(Engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# Create a single unity file that includes all sources
set(ENGINE_UNITY_FILE "${CMAKE_CURRENT_BINARY_DIR}/EngineUnity.cpp")



# Generate the unity file
file(WRITE ${ENGINE_UNITY_FILE} "// Auto-generated unity build file\n\n")
foreach(SRC_FILE ${Engine_LIB_SOURCES})
    file(APPEND ${ENGINE_UNITY_FILE} "#include \"${SRC_FILE}\"\n")
endforeach()

# Create library with just the unity file and excluded sources
add_library(Engine STATIC ${ENGINE_UNITY_FILE})

# Everything below this is EXACTLY THE SAME as your original file
# Include directories (relative to Engine/)
target_include_directories(Engine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../external/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Source/ImGui/"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Source/Utilities/CommonUtilities/include"
   #"${CMAKE_CURRENT_SOURCE_DIR}/../../external/include/physx/"
    "${CMAKE_CURRENT_SOURCE_DIR}/GraphicsEngine/"
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/"
)

target_compile_definitions(Engine PRIVATE OPENGL)

add_definitions(-D_DEBUG)

target_link_libraries(Engine PRIVATE CommonUtilities)
target_link_libraries(Engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../external/lib/glfw3.lib")
target_link_libraries(Engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../external/lib/glfw3_mt.lib")

set_target_properties(Engine PROPERTIES 
    OUTPUT_NAME_DEBUG "Engine"
    OUTPUT_NAME_RELEASE "Engine"
)
