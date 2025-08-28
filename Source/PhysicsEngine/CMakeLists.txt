cmake_minimum_required(VERSION "3.19.2")

project(PhysicsEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(PhysicsEngine STATIC)

file(GLOB_RECURSE PhysicsEngine_ENGINE_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    #"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" 
)

list(REMOVE_ITEM PhysicsEngine_ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/PhysicsEngine.cpp")

target_sources(PhysicsEngine PRIVATE ${PhysicsEngine_ENGINE_SOURCES})

# Include directories (relative to Engine/)
target_include_directories(PhysicsEngine PRIVATE
    "${CMAKE_SOURCE_DIR}/external/include/"
    #"${CMAKE_SOURCE_DIR}/external/include/physx/"
    "${CMAKE_SOURCE_DIR}/Source/ImGui/"
    "${CMAKE_SOURCE_DIR}/Source/Utilities/CommonUtilities/include"
    "${CMAKE_SOURCE_DIR}/Source/Engine/GraphicsEngine/"
    "${CMAKE_SOURCE_DIR}/Source/Engine/GameEngine/"
)

target_compile_definitions(PhysicsEngine PRIVATE OPENGL $<$<CONFIG:Debug>:_DEBUG>)
    if (EMSCRIPTEN)
        target_compile_definitions(PhysicsEngine PRIVATE BOOST_ASIO_DISABLE_SIGNAL_BLOCKING)
    endif(EMSCRIPTEN)

#target_link_directories(PhysicsEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../external/lib/debug/PhysX/")

#target_link_libraries(PhysicsEngine PRIVATE LowLevel_static_64)
#target_link_libraries(PhysicsEngine PRIVATE LowLevelAABB_static_64)
#target_link_libraries(PhysicsEngine PRIVATE LowLevelDynamics_static_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysX_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXCharacterKinematic_static_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXCommon_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXCooking_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXExtensions_static_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXFoundation_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXPvdSDK_static_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXTask_static_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXVehicle_static_64)
#target_link_libraries(PhysicsEngine PRIVATE PhysXVehicle2_static_64)
#target_link_libraries(PhysicsEngine PRIVATE SceneQuery_static_64)
#target_link_libraries(PhysicsEngine PRIVATE SimulationController_static_64)

target_link_libraries(PhysicsEngine PRIVATE CommonUtilities)

set_target_properties(PhysicsEngine PROPERTIES 
    OUTPUT_NAME_DEBUG "PhysicsEngine"
    OUTPUT_NAME_RELEASE "PhysicsEngine"
)
