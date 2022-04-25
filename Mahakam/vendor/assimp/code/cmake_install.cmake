# Install script for directory: /home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/lib/libassimp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/anim.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/aabb.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/ai_assert.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/camera.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/color4.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/color4.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/config.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/ColladaMetaData.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/commonMetaData.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/defs.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/cfileio.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/light.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/material.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/material.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/matrix3x3.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/matrix3x3.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/matrix4x4.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/matrix4x4.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/mesh.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/pbrmaterial.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/GltfMaterial.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/postprocess.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/quaternion.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/quaternion.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/scene.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/metadata.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/texture.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/types.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/vector2.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/vector2.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/vector3.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/vector3.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/version.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/cimport.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/importerdesc.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Importer.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/IOStream.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/IOSystem.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Logger.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/LogStream.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/NullLogger.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/cexport.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Exporter.hpp"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/DefaultIOStream.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/DefaultIOSystem.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SceneCombiner.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/fast_atof.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/qnan.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/BaseImporter.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Hash.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/ParsingUtils.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/StreamReader.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/StreamWriter.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/StringComparison.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/StringUtils.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SGSpatialSort.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/GenericProperty.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SpatialSort.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SmallVector.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SmoothingGroups.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/SmoothingGroups.inl"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/StandardShapes.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/RemoveComments.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Subdivision.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Vertex.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/LineSplitter.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/TinyFormatter.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Profiler.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/LogAux.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Bitmap.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/XMLTools.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/IOStreamBuffer.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/CreateAnimMesh.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/XmlParser.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/BlobIOSystem.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/MathFunctions.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Exceptional.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/ByteSwapper.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/home/christian/CPPProjects/Mahakam/Mahakam/vendor/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

