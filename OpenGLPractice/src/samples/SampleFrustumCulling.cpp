#include "SampleFrustumCulling.h"
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Log.h"
#include "Entity.h"

namespace sample
{

	SampleFrustumCulling::SampleFrustumCulling()
		: Done(false)
	{
		ourShader = std::make_unique<Renderer::Shader>("res/shaders/frustumculling/ModelLoading.vert",
			"res/shaders/frustumculling/ModelLoading.frag");

		model = std::make_unique<Model>("res/models/planet/planet.obj");

		ourEntity = std::make_unique<Entity>(model.get());

		ourEntity->transform.setLocalPosition({ 0, 0, 0 });
		const float scale = 1.0f;
		ourEntity->transform.setLocalScale({ scale, scale, scale });

		Entity* lastEntity = ourEntity.get();
		for (unsigned int x = 0; x < 20; ++x)
		{
			for (unsigned int z = 0; z < 20; ++z)
			{
				ourEntity->addChild(model.get());
				lastEntity = ourEntity->children.back().get();

				// Set transform values
				lastEntity->transform.setLocalPosition({ x * 10.f - 100.f,  0.f, z * 10.f - 100.f });
			}
		}
		ourEntity->updateSelfAndChild();
	}

	SampleFrustumCulling::~SampleFrustumCulling()
	{

	}

	void SampleFrustumCulling::OnUpdate(float deltaTime)
	{

	}

	void SampleFrustumCulling::OnRender(const Camera& camera, RenderScene* scenebuffer)
	{
		if (!Done || scenebuffer->HasChanged())
		{
			Init(scenebuffer);
		}

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)m_Width / (float)m_Height,
			0.1f, 100.0f);
		const Frustum camFrustum = createFrustumCamera(camera, (float)m_Width / (float)m_Height, glm::radians(camera.GetZoom()),
			0.1f, 100.0f);

		//

		glm::mat4 view = camera.GetViewMatrix();

		ourShader->SetUniformMat4f("projection", 1, projection);
		ourShader->SetUniformMat4f("view", 1, view);

		// draw scene graph
		unsigned int total = 0, display = 0;
		ourEntity->drawSelfAndChild(camFrustum, *ourShader, display, total);
		RD_INFO("Total process in CPU: {0} / Total send to GPU : {1}", total, display);

		ourEntity->updateSelfAndChild();

		scenebuffer->BlitFramebuffer();
	}

	void SampleFrustumCulling::OnImGuiRenderer()
	{

	}

	void SampleFrustumCulling::Init(RenderScene* scenebuffer)
	{
		m_Width = scenebuffer->GetWidth();
		m_Height = scenebuffer->GetHeight();
		Done = true;
	}

}