#include "SampleRenderTerrainCPU.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include "Log.h"

namespace sample
{

	SampleRenderTerrainCPU::SampleRenderTerrainCPU()
		: Done(false), displayGrayScale(false), useWireFrame(false)
	{
		// build and compile shader program
		MapShader = std::make_unique<Renderer::Shader>("res/shaders/terrain/CPUHeight.vert",
			"res/shaders/terrain/CPUHeight.frag");

		// load image
		int width, height, nrChannels;
		unsigned char* data = stbi_load("res/textures/heightmaps/iceland_heightmap.png", &width, &height, &nrChannels, 0);
		if (data)
		{
			RD_INFO("Loaded heightmap of size {0} x {1}", height, width);
		}
		else
		{
			RD_ERROR("Failed to load texture");
		}

		// set up vertex data (and buffer(s)) and configure vertex attributes
		std::vector<float> vertices;
		vertices.reserve(width * height * 3);
		float yScale = 64.0f / 256.0f, yShift = 16.0f;
		int rez = 1;
		unsigned bytePerPixel = nrChannels;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
				unsigned char y = pixelOffset[0];

				// vertex
				vertices.push_back(-height / 2.0f + height * i / (float)height); // vx
				vertices.push_back((int)y * yScale - yShift);   // vy
				vertices.push_back(-width / 2.0f + width * j / (float)width);    // vz
			}
		}
		stbi_image_free(data);

		std::vector<unsigned> indices;
		indices.reserve((height - 1) * width * 2);
		for (unsigned i = 0; i < height - 1; i += rez)
		{
			for (unsigned j = 0; j < width; j += rez)
			{
				for (unsigned k = 0; k < 2; ++k)
				{
					indices.push_back(j + width * (i + k * rez));
				}
			}
		}

		numStrips = (height - 1) / rez;
		numTrisPerStrip = (width / rez) * 2 - 2;
		RD_INFO("Created lattice of {0} strips with {1} triangles each", numStrips, numTrisPerStrip);
		RD_INFO("Created {0} triangles total", (numStrips * numTrisPerStrip));

		// cube's VAO
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &terrainIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
	}

	SampleRenderTerrainCPU::~SampleRenderTerrainCPU()
	{
		glDeleteVertexArrays(1, &terrainVAO);
		glDeleteBuffers(1, &terrainVBO);
		glDeleteBuffers(1, &terrainIBO);
	}

	void SampleRenderTerrainCPU::OnUpdate(float deltaTime)
	{

	}

	void SampleRenderTerrainCPU::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}

		scenebuffer->Bind();
		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setting uniforms/drawing objects
		MapShader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Width,
			0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		MapShader->SetUniformMat4f("projection", 1, projection);
		MapShader->SetUniformMat4f("view", 1, view);

		glm::mat4 model = glm::mat4(1.0f);
		MapShader->SetUniformMat4f("model", 1, model);

		// render the cube
		glBindVertexArray(terrainVAO);
		if (useWireFrame) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (unsigned strip = 0; strip < numStrips; ++strip)
		{
			glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip));
		}

		scenebuffer->Unbind();
	}

	void SampleRenderTerrainCPU::OnImGuiRenderer()
	{
		ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.7f, 1.0f), "Rendering Terrain via CPU");
		ImGui::Checkbox("Enable Wire Frame", &useWireFrame);
	}

	void SampleRenderTerrainCPU::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;
	}

}