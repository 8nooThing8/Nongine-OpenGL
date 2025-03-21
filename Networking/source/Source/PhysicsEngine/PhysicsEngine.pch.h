// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.
#pragma once
#pragma message("Compiling PhysicsEngine.pch.h\n")

// add headers that you want to pre-compile here
#include "PhysicsEngine.h"
#include <physx\PxPhysics.h>
#include <physx\PxPhysicsAPI.h>
#include <physx\PxPhysicsSerialization.h>
#include <physx\PxPhysXConfig.h>

#include "../Utilities/Logger/Logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogPhysicsEngine, PhysicsEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogPhysicsEngine, PhysicsEngine, Warning);
#endif

#define GELOG(Verbosity, Message, ...) LOG(LogPhysicsEngine, Verbosity, Message, ##__VA_ARGS__)
