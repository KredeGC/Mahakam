#pragma once

#include "Core.h"

// Function declarations which should be reachable in DLLs
#define MH_DECLARE_FUNC(func, returnType, ...) static returnType func(__VA_ARGS__);
#define MH_DEFINE_FUNC(func, returnType, ...) returnType func(__VA_ARGS__)