#include "SampleIBLSpecularTexture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include "Log.h"

namespace sample
{

	SampleIBLSpecularTexture::SampleIBLSpecularTexture()
		: cubeVAO(0), cubeVBO(0), quadVAO(0), quadVBO(0), sphereVAO(0), indexCount(0),
		nrRows(7), nrColumns(7), spacing(2.5f), maxMipLevels(5)
	{
		// configure OpenGL state
		// set depth function to less than AND equal for skybox depth trick.
		glDepthFunc(GL_LEQUAL);
		// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// build and compile shaders
		pbrShader = std::make_unique<Renderer::Shader>("res/shaders/ibltexture/PBR.vert",
			"res/shaders/ibltexture/PBR.frag");
		equirectangularToCubemapShader = std::make_unique<Renderer::Shader>("res/shaders/ibltexture/CubeMap.vert",
			"res/shaders/ibltexture/Equirectangular.frag");
		irradianceShader = std::make_unique<Renderer::Shader>("res/shaders/ibltexture/CubeMap.vert",
			"res/shaders/ibltexture/IrradianceConvolution.frag");
		prefilterShader = std::make_unique<Renderer::Shader>("res/shaders/ibltexture/CubeMap.vert",
			"res/shaders/ibltexture/Prefilter.frag");
		brdfShader = std::make_unique<Renderer::Shader>("res/shaders/ibltexture/Brdf.vert",
			"res/shaders/ibltexture/Brdf.frag");
		backgroundShader = std::make_unique<Renderer::Shader>("res/shaders/ibltexture/Background.vert",
			"res/shaders/ibltexture/Background.frag");

		pbrShader->Bind();
		pbrShader->SetUniform1i("irradianceMap", 0);
		pbrShader->SetUniform1i("prefilterMap", 1);
		pbrShader->SetUniform1i("brdfLUT", 2);
		pbrShader->SetUniform1i("albedoMap", 3);
		pbrShader->SetUniform1i("normalMap", 4);
		pbrShader->SetUniform1i("metallicMap", 5);
		pbrShader->SetUniform1i("roughnessMap", 6);
		pbrShader->SetUniform1i("aoMap", 7);

		backgroundShader->Bind();
		backgroundShader->SetUniform1i("environmentMap", 0);

		// load PBR material textures
		// --------------------------
		// rusted iron
		ironAlbedoMap = loadTexture("res/textures/pbr/rusted_iron/albedo.png");
		ironNormalMap = loadTexture("res/textures/pbr/rusted_iron/normal.png");
		ironMetallicMap = loadTexture("res/textures/pbr/rusted_iron/metallic.png");
		ironRoughnessMap = loadTexture("res/textures/pbr/rusted_iron/roughness.png");
		ironAOMap = loadTexture("res/textures/pbr/rusted_iron/ao.png");

		// gold
		goldAlbedoMap = loadTexture("res/textures/pbr/gold/albedo.png");
		goldNormalMap = loadTexture("res/textures/pbr/gold/normal.png");
		goldMetallicMap = loadTexture("res/textures/pbr/gold/metallic.png");
		goldRoughnessMap = loadTexture("res/textures/pbr/gold/roughness.png");
		goldAOMap = loadTexture("res/textures/pbr/gold/ao.png");

		// grass
		grassAlbedoMap = loadTexture("res/textures/pbr/grass/albedo.png");
		grassNormalMap = loadTexture("res/textures/pbr/grass/normal.png");
		grassMetallicMap = loadTexture("res/textures/pbr/grass/metallic.png");
		grassRoughnessMap = loadTexture("res/textures/pbr/grass/roughness.png");
		grassAOMap = loadTexture("res/textures/pbr/grass/ao.png");

		// plastic
		plasticAlbedoMap = loadTexture("res/textures/pbr/plastic/albedo.png");
		plasticNormalMap = loadTexture("res/textures/pbr/plastic/normal.png");
		plasticMetallicMap = loadTexture("res/textures/pbr/plastic/metallic.png");
		plasticRoughnessMap = loadTexture("res/textures/pbr/plastic/roughness.png");
		plasticAOMap = loadTexture("res/textures/pbr/plastic/ao.png");

		// wall
		wallAlbedoMap = loadTexture("res/textures/pbr/wall/albedo.png");
		wallNormalMap = loadTexture("res/textures/pbr/wall/normal.png");
		wallMetallicMap = loadTexture("res/textures/pbr/wall/metallic.png");
		wallRoughnessMap = loadTexture("res/textures/pbr/wall/roughness.png");
		wallAOMap = loadTexture("res/textures/pbr/wall/ao.png");
		
		// lights
		lightPositions.reserve(5);
		lightPositions.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));
		lightPositions.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
		lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
		lightPositions.push_back(glm::vec3(10.0f, -10.0f, 10.0f));

		lightColors.reserve(5);
		lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

		// pbr: setup framebuffer
		// ----------------------
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		// pbr: load the HDR environment map
		// ---------------------------------
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		float* data = stbi_loadf("res/textures/hdr/newport_loft.hdr", &width, &height, &nrComponents, 0);
		if (data)
		{
			glGenTextures(1, &hdrTexture);
			glBindTexture(GL_TEXTURE_2D, hdrTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			RD_ERROR("Failed to load HDR image");
		}

		// pbr: setup cubemap to render to and attach to framebuffer
		// ---------------------------------------------------------
		glGenTextures(1, &envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB,
				GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// pbr: setup projection and view matrices for capturing data onto the 6 cubemap face directions
		captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		captureViews.reserve(6);
		captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		// pbr: convert HDR equirectangular environment map to cubemap equivalent
		equirectangularToCubemapShader->Bind();
		equirectangularToCubemapShader->SetUniform1i("equirectangularMap", 0);
		equirectangularToCubemapShader->SetUniformMat4f("projection", 1, captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);

		glViewport(0, 0, 512, 512);  // configure the viewport to the capture dimensions
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			equirectangularToCubemapShader->SetUniformMat4f("view", 1, captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
		// --------------------------------------------------------------------------------
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, 
				GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		// pbr: solve diffuse integral by convolution to create an irradiance(cube)map.
		// -----------------------------------------------------------------------------
		irradianceShader->Bind();
		irradianceShader->SetUniform1i("environmentMap", 0);
		irradianceShader->SetUniformMat4f("projection", 1, captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceShader->SetUniformMat4f("view", 1, captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
		// --------------------------------------------------------------------------------
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,
				GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
		// ----------------------------------------------------------------------------------------------------
		prefilterShader->Bind();
		prefilterShader->SetUniform1i("environmentMap", 0);
		prefilterShader->SetUniformMat4f("projection", 1, captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader->SetUniform1f("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader->SetUniformMat4f("view", 1, captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// pbr: generate a 2D LUT from the BRDF equations used.
		// ----------------------------------------------------
		glGenTextures(1, &brdfLUTTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, 512, 512);
		brdfShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// before rendering, configure the viewport
		glViewport(0, 0, m_Width, m_Height);

	}

	SampleIBLSpecularTexture::~SampleIBLSpecularTexture()
	{
		glDepthFunc(GL_LESS);
		glDeleteVertexArrays(1, &sphereVAO);
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteVertexArrays(1, &cubeVAO);

		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &cubeVBO);

		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);

		glDeleteTextures(1, &envCubemap);
		glDeleteTextures(1, &irradianceMap);
		glDeleteTextures(1, &prefilterMap);
		glDeleteTextures(1, &brdfLUTTexture);
		glDeleteTextures(1, &hdrTexture);

		glDeleteTextures(1, &ironAlbedoMap);
		glDeleteTextures(1, &ironNormalMap);
		glDeleteTextures(1, &ironMetallicMap);
		glDeleteTextures(1, &ironRoughnessMap);
		glDeleteTextures(1, &ironAOMap);

		glDeleteTextures(1, &goldAlbedoMap);
		glDeleteTextures(1, &goldNormalMap);
		glDeleteTextures(1, &goldMetallicMap);
		glDeleteTextures(1, &goldRoughnessMap);
		glDeleteTextures(1, &goldAOMap);

		glDeleteTextures(1, &grassAlbedoMap);
		glDeleteTextures(1, &grassNormalMap);
		glDeleteTextures(1, &grassMetallicMap);
		glDeleteTextures(1, &grassRoughnessMap);
		glDeleteTextures(1, &grassAOMap);
		// ...
	}

	void SampleIBLSpecularTexture::OnUpdate(float deltaTime)
	{

	}

	void SampleIBLSpecularTexture::OnRender(const Camera& camera)
	{
		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render scene, supplying the convoluted irradiance map to the final shader.
		// ------------------------------------------------------------------------------------------
		pbrShader->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		pbrShader->SetUniformMat4f("projection", 1, projection);
		pbrShader->SetUniformMat4f("view", 1, view);
		pbrShader->SetUniformVec3f("camPos", camera.GetPosition());

		// bind pre-computed IBL data
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		// rusted iron
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ironAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ironNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ironMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, ironRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, ironAOMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
		pbrShader->SetUniformMat4f("model", 1, model);
		renderSphere();

		// gold
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, goldAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, goldNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, goldMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, goldRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, goldAOMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
		pbrShader->SetUniformMat4f("model", 1, model);
		renderSphere();

		// grass
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, grassAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, grassNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, grassMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, grassRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, grassAOMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
		pbrShader->SetUniformMat4f("model", 1, model);
		renderSphere();

		// plastic
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, plasticAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, plasticNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, plasticMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, plasticRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, plasticAOMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
		pbrShader->SetUniformMat4f("model", 1, model);
		renderSphere();

		// wall
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, wallAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, wallNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, wallMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, wallRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, wallAOMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
		pbrShader->SetUniformMat4f("model", 1, model);
		renderSphere();

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			pbrShader->SetUniformVec3f("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader->SetUniformVec3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			pbrShader->SetUniformMat4f("model", 1, model);
			renderSphere();
		}

		// render skybox (render as last to prevent overdraw)
		backgroundShader->Bind();
		backgroundShader->SetUniformMat4f("view", 1, view);
		backgroundShader->SetUniformMat4f("projection", 1, projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
		//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
		renderCube();

		// render BRDF map to screen
		//brdfShader.Use();
		//renderQuad();

	}

	void SampleIBLSpecularTexture::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.7f, 1.0f), "IBL Specular Texture");
	}

	void SampleIBLSpecularTexture::renderSphere()
	{
		if (sphereVAO == 0)
		{
			glGenVertexArrays(1, &sphereVAO);

			unsigned int vbo, ebo;
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			std::vector<glm::vec3> positions;
			std::vector<glm::vec2> uv;
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = 64;
			const unsigned int Y_SEGMENTS = 64;
			const float PI = 3.14159265359f;
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					positions.push_back(glm::vec3(xPos, yPos, zPos));
					uv.push_back(glm::vec2(xSegment, ySegment));
					normals.push_back(glm::vec3(xPos, yPos, zPos));
				}
			}

			bool oddRow = false;
			for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
			{
				if (!oddRow) // even rows: y == 0, y == 2; and so on
				{
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
					{
						indices.push_back(y * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else
				{
					for (int x = X_SEGMENTS; x >= 0; --x)
					{
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}
			indexCount = static_cast<GLsizei>(indices.size());

			std::vector<float> data;
			for (unsigned int i = 0; i < positions.size(); ++i)
			{
				data.push_back(positions[i].x);
				data.push_back(positions[i].y);
				data.push_back(positions[i].z);
				if (normals.size() > 0)
				{
					data.push_back(normals[i].x);
					data.push_back(normals[i].y);
					data.push_back(normals[i].z);
				}
				if (uv.size() > 0)
				{
					data.push_back(uv[i].x);
					data.push_back(uv[i].y);
				}
			}
			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			unsigned int stride = (3 + 2 + 3) * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	}

	void SampleIBLSpecularTexture::renderCube()
	{
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void SampleIBLSpecularTexture::renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	unsigned int SampleIBLSpecularTexture::loadTexture(const char* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			RD_ERROR("Texture failed to load at path: {0}", path);
			stbi_image_free(data);
		}

		return textureID;
	}

	unsigned int SampleIBLSpecularTexture::m_Width = 800;

	unsigned int SampleIBLSpecularTexture::m_Height = 600;

}