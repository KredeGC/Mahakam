#include <emmintrin.h>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <robin_hood.h>

#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/KeyCodes.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/MouseButtonCodes.h"
#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/Utility.h"

#include "Mahakam/Events/ApplicationEvent.h"
#include "Mahakam/Events/Event.h"
#include "Mahakam/Events/KeyEvent.h"
#include "Mahakam/Events/MouseEvent.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#ifdef MH_PLATFORM_WINDOWS
#include <Windows.h>
#endif