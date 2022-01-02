#include "mhpch.h"
#include "OpenGLTexture.h"

#include "OpenGLShader.h"

#include "Mahakam/Renderer/Mesh.h"

#include <stb_image.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mahakam
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath) : filepath(filepath)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(filepath.c_str(), &w, &h, &channels, 0);

		MH_CORE_ASSERT(data, "Failed to load image!");

		width = w;
		height = h;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		MH_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glBindTexture(GL_TEXTURE_2D, rendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &rendererID);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}



	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& faces)
	{
		glGenTextures(1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);

		int w, h, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &nrChannels, 0);

			MH_CORE_ASSERT(data, "Failed to load image!");

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);

			stbi_image_free(data);
		}

		int numMips = 1 + glm::floor(glm::log2(glm::max((float)w, (float)h)));

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMips);

		width = w;
		height = h;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::string& filepath) : filepath(filepath)
	{
		bool equirectangular = true;

		stbi_set_flip_vertically_on_load(true);
		int w, h, channels;
		float* data = stbi_loadf(filepath.c_str(), &w, &h, &channels, 0);

		int resolution = 32;

		MH_CORE_ASSERT(data, "Failed to load image!");

		width = w;
		height = h;

		uint32_t hdrID;
		glGenTextures(1, &hdrID);
		glBindTexture(GL_TEXTURE_2D, hdrID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);


		// Create framebuffer
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


		// Create cubemap
		glGenTextures(1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);
		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
				resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		// Create matrices
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};


		// Convert HDR equirectangular environment map to cubemap equivalent
		OpenGLShader equiToCubeShader("assets/shaders/Cubemap.glsl");
		equiToCubeShader.bind();
		equiToCubeShader.setUniformInt("equirectangularMap", 0);
		equiToCubeShader.setUniformMat4("projection", captureProjection);
		glBindTextureUnit(0, hdrID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrID);

		Ref<Mesh> cube = Mesh::createCube(2);
		cube->bind();

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glViewport(0, 0, resolution, resolution); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			equiToCubeShader.setUniformMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rendererID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDrawElements(GL_TRIANGLES, cube->getIndexCount(), GL_UNSIGNED_INT, nullptr);
		}

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);


		// Delete buffer
		glDeleteTextures(1, &hdrID);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteBuffers(1, &captureFBO);
		glDeleteBuffers(1, &captureRBO);
		glDeleteTextures(1, &rendererID);
	}

	void OpenGLTextureCube::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}
}