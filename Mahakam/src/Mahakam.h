#pragma once

// Core
#include "Mahakam/Core/Application.h"
#include "Mahakam/Core/AssetDatabase.h"
#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Instrumentor.h"
#include "Mahakam/Core/KeyCodes.h"
#include "Mahakam/Core/Layer.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Math/Math.h"
#include "Mahakam/Core/MouseButtonCodes.h"
#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/Utility.h"
#include "Mahakam/Core/Window.h"

// Events
#include "Mahakam/Events/Event.h"
#include "Mahakam/Events/ApplicationEvent.h"
#include "Mahakam/Events/KeyEvent.h"
#include "Mahakam/Events/MouseEvent.h"

// ImGui
#include "Mahakam/ImGui/GUI.h"
#include "Mahakam/ImGui/ImGuiLayer.h"

// Math
#include "Mahakam/Math/Math.h"

// Rendering
#include "Mahakam/Renderer/Animation.h"
#include "Mahakam/Renderer/Animator.h"
#include "Mahakam/Renderer/Bone.h"
#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/Camera.h"
#include "Mahakam/Renderer/ComputeShader.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Light.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/RenderBuffer.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/RenderPasses.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

//Scene
#include "Mahakam/Scene/ComponentRegistry.h"
#include "Mahakam/Scene/Components.h"
#include "Mahakam/Scene/Entity.h"
#include "Mahakam/Scene/Scene.h"
#include "Mahakam/Scene/SceneSerializer.h"
#include "Mahakam/Scene/ScriptableEntity.h"