#include <emmintrin.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#ifndef MH_DEBUG
#include <robin_hood/robin_hood.h>
#else
#include <unordered_map>
#include <unordered_set>
#endif

#include "Mahakam/Asset/AssetImporter.h"
#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Core/Core.h"
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
#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Window.h"

#include "Mahakam/Events/ApplicationEvent.h"
#include "Mahakam/Events/Event.h"
#include "Mahakam/Events/KeyEvent.h"
#include "Mahakam/Events/MouseEvent.h"

#include "Mahakam/ImGui/GUI.h"
#include "Mahakam/ImGui/ImGuiLayer.h"

#include "Mahakam/Math/Bounds.h"
#include "Mahakam/Math/Math.h"

#include <entt/entt.hpp>

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_int4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <yaml-cpp/yaml.h>

#ifdef MH_BUILD
#include <miniaudio/miniaudio.h>
#include <steamaudio/phonon.h>
#include <tiny_gltf/tiny_gltf.h>
#endif