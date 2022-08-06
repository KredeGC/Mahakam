#include <emmintrin.h>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#ifndef MH_DEBUG
#include <robin_hood.h>
#else
#include <unordered_map>
#endif

#include "Mahakam/Asset/Asset.h"
#include "Mahakam/Asset/AssetImporter.h"
#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Core/Application.h"
#include "Mahakam/Core/Frustum.h"
#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Instrumentor.h"
#include "Mahakam/Core/KeyCodes.h"
#include "Mahakam/Core/Layer.h"
#include "Mahakam/Core/LayerStack.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/MouseButtonCodes.h"
#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/Random.h"
#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/Utility.h"
#include "Mahakam/Core/Window.h"

#include "Mahakam/Events/ApplicationEvent.h"
#include "Mahakam/Events/Event.h"
#include "Mahakam/Events/KeyEvent.h"
#include "Mahakam/Events/MouseEvent.h"

#include "Mahakam/ImGui/GUI.h"
#include "Mahakam/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <yaml-cpp/yaml.h>

#ifdef MH_BUILD
#include <miniaudio/miniaudio.h>
#include <steamaudio/phonon.h>
#endif

#ifdef MH_PLATFORM_WINDOWS
#include <Windows.h>
#endif