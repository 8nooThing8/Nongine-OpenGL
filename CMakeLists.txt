cmake_minimum_required(VERSION "3.19.2")

project(NonGine LANGUAGES CXX)

enable_language(C)

include("${CMAKE_SOURCE_DIR}/Settings/UtilityFunctions.cmake")

# Set executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)

add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/Source/Utilities/CommonUtilities/glad.c" )
set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/Source/Utilities/CommonUtilities/glad.c" PROPERTIES LANGUAGE C)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)
set_target_properties(glad PROPERTIES C_STANDARD 11)


add_subdirectory("Source/Utilities/CommonUtilities")
add_subdirectory("Source/Engine")
add_subdirectory("Source/ImGui")
add_subdirectory("Source/Networking/Client")
add_subdirectory("Source/Networking/Server")
add_subdirectory("Source/PhysicsEngine")

add_subdirectory("Source/StartServer")
add_subdirectory("Source/FileCollection")

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/external/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/ImGui/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Utilities/CommonUtilities/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/include/physx/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/GraphicsEngine/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/GameEngine/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Application/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Networking/"
)



project(Game_Engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
AddUnityTarget(Game_Engine "${CMAKE_CURRENT_SOURCE_DIR}/Source/Application/Main.cpp" ${GAME_ENGINE_SOURCES})
#add_executable(Game_Engine "${CMAKE_CURRENT_SOURCE_DIR}/Source/Application/Main.cpp" ${GAME_ENGINE_SOURCES})

target_compile_definitions(Game_Engine PRIVATE IMGUISETTINGSDIR="${CMAKE_SOURCE_DIR}/Settings/imgui.ini")

target_compile_definitions(Game_Engine PRIVATE OPENGL $<$<CONFIG:Debug>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)

if(EMSCRIPTEN)
    target_compile_definitions(Game_Engine PRIVATE NONEMSCRIPTEN)

    message(STATUS "Building for Emscripten / WebAssembly")

    set(CMAKE_EXECUTABLE_SUFFIX ".html") # optional, .js/.html output
    set(EMSCRIPTEN_FLAGS "-s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s ASSERTIONS=2 -s STACK_OVERFLOW_CHECK=2 -s GL_DEBUG=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s GL_ASSERTIONS=1 -s EXPORT_ALL=1 -s NO_EXIT_RUNTIME=1 -s USE_WEBGL2=1 -s STACK_SIZE=5MB")
    set_target_properties(Game_Engine PROPERTIES LINK_FLAGS "${EMSCRIPTEN_FLAGS}")
    set(EMSCRIPTEN_PRELOAD "--preload-file ${CMAKE_SOURCE_DIR}/Assets/Shaders/QuadVS.glsl")
else()
    # Native Windows/Linux linking
    if(WIN32)
        target_compile_definitions(Game_Engine PRIVATE NONWIN32)
        target_link_libraries(Game_Engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/glfw3.lib")
        target_link_libraries(Game_Engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external/lib/glfw3_mt.lib")
    elseif(UNIX)
        target_compile_definitions(Game_Engine PRIVATE NONUNIX)
        target_link_libraries(Game_Engine PRIVATE glfw OpenGL::GL)
    endif()
endif()


find_package(OpenGL REQUIRED)

target_link_directories(Game_Engine PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/external/lib/" 
	$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Lib/Debug/>
	$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/Lib/Release/>
)

target_link_libraries(Game_Engine PRIVATE CommonUtilities Engine ImGui PhysicsEngine glad OpenGL::GL)

set_target_properties(Game_Engine PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/Bin")

set_target_properties(Game_Engine PROPERTIES OUTPUT_NAME_DEBUG "GameEngine_debug")
set_target_properties(Game_Engine PROPERTIES OUTPUT_NAME_RELEASE "GameEngine_release")

make_directory("${CMAKE_SOURCE_DIR}/Assets")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Assets" "${CMAKE_SOURCE_DIR}/Bin/Assets")