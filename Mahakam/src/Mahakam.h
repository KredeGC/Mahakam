#pragma once

// Core
#include "Mahakam/Core/Application.h"
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

// Asset
#include "Mahakam/Asset/Asset.h"
#include "Mahakam/Asset/AssetDatabase.h"
#include "Mahakam/Asset/AssetImporter.h"
#include "Mahakam/Asset/MaterialAssetImporter.h"
#include "Mahakam/Asset/ShaderAssetImporter.h"
#include "Mahakam/Asset/SoundAssetImporter.h"
#include "Mahakam/Asset/TextureAssetImporter.h"

// Audio
#include "Mahakam/Audio/AudioContext.h"
#include "Mahakam/Audio/AudioEngine.h"
#include "Mahakam/Audio/AudioSource.h"
#include "Mahakam/Audio/Sound.h"

// Editor
#ifndef MH_RUNTIME
#include "Mahakam/Editor/EditorCamera.h"
#include "Mahakam/Editor/EditorWindow.h"
#include "Mahakam/Editor/EditorWindowRegistry.h"
#include "Mahakam/Editor/SceneManager.h"
#include "Mahakam/Editor/Selection.h"
#endif

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

// Physics
#include "Mahakam/Physics/Colliders/BoxCollider.h"
#include "Mahakam/Physics/Colliders/Collider.h"
#include "Mahakam/Physics/Colliders/SphereCollider.h"
#include "Mahakam/Physics/PhysicsContext.h"
#include "Mahakam/Physics/PhysicsEngine.h"
#include "Mahakam/Physics/Rigidbody.h"

// Rendering
#include "Mahakam/Renderer/RenderPasses/GeometryRenderPass.h"
#include "Mahakam/Renderer/RenderPasses/LightingRenderPass.h"
#include "Mahakam/Renderer/RenderPasses/ParticleRenderPass.h"
#include "Mahakam/Renderer/RenderPasses/TonemappingRenderPass.h"
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
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

//Scene
#include "Mahakam/Scene/Components/AnimatorComponent.h"
#include "Mahakam/Scene/Components/AudioListenerComponent.h"
#include "Mahakam/Scene/Components/AudioSourceComponent.h"
#include "Mahakam/Scene/Components/CameraComponent.h"
#include "Mahakam/Scene/Components/LightComponent.h"
#include "Mahakam/Scene/Components/MeshComponent.h"
#include "Mahakam/Scene/Components/ParticleSystemComponent.h"
#include "Mahakam/Scene/Components/TagComponent.h"
#include "Mahakam/Scene/Components/TransformComponent.h"
#include "Mahakam/Scene/ComponentRegistry.h"
#include "Mahakam/Scene/Entity.h"
#include "Mahakam/Scene/Scene.h"
#include "Mahakam/Scene/SceneSerializer.h"