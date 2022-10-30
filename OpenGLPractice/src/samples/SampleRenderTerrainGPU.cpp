#include "SampleRenderTerrainGPU.h"
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

namespace sample
{

	SampleRenderTerrainGPU::SampleRenderTerrainGPU()
		: Done(false), useWireFrame(false), displayGrayScale(false), NUM_PATCH_PTS(4)
	{
		GLint maxTessLevel;
		glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
		RD_INFO("Max available tess level: {0}", maxTessLevel);

		// build and compile our shader program
		MapShader = std::make_unique<Renderer::ShaderN>("res/shaders/terrain/GPUHeight.vert", "res/shaders/terrain/GPUHeight.frag",
			nullptr, "res/shaders/terrain/GPUHeight.tesc", "res/shaders/terrain/GPUHeight.tese");

		// load and create a texture
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		int width, height, nrChannels;
		unsigned char* data = stbi_load("res/textures/heightmaps/iceland_heightmap.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			MapShader->setInt("heightMap", 0);
			RD_INFO("Loaded HeightMap of size {0} x {1}", height, width);
		}
		else
		{
			RD_ERROR("Failed to load texture");
		}
		stbi_image_free(data);

		// set up vertex data (and buffer(s)) and configure vertex attributes
		std::vector<float> vertices;
		rez = 20;
		vertices.reserve(rez * rez * 20);
		for (unsigned i = 0; i <= rez - 1; i++)
		{
			for (unsigned j = 0; j <= rez - 1; j++)
			{
				vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
				vertices.push_back(i / (float)rez); // u
				vertices.push_back(j / (float)rez); // v

				vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
				vertices.push_back((i + 1) / (float)rez); // u
				vertices.push_back(j / (float)rez); // v

				vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
				vertices.push_back(i / (float)rez); // u
				vertices.push_back((j + 1) / (float)rez); // v

				vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
				vertices.push_back((i + 1) / (float)rez); // u
				vertices.push_back((j + 1) / (float)rez); // v
			}
		}
		RD_INFO("Loaded {0} patches of 4 control points each", rez * rez);
		RD_INFO("Processing {0} vertices in vertices in vertex shader", rez * rez * 4);

		// configure the cube's VAO (and terrainVBO)
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texCoord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

	}

	SampleRenderTerrainGPU::~SampleRenderTerrainGPU()
	{

	}

	void SampleRenderTerrainGPU::OnUpdate(float deltaTime)
	{

	}

	void SampleRenderTerrainGPU::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}

		// render
		scenebuffer->Bind();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// activate shader when setting uniforms/drawing objects
		MapShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		MapShader->setMat4("projection", projection);
		MapShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		MapShader->setMat4("model", model);
		// render the terrain
		if (useWireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(terrainVAO);
		glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);

		scenebuffer->Unbind();
	}

	void SampleRenderTerrainGPU::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.1f, 0.9f, 0.8f, 1.0f), "Render Terrain via Tessellation Shader");
		ImGui::Checkbox("Enable Wire Frame", &useWireFrame);
	}

	void SampleRenderTerrainGPU::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;
	}

}