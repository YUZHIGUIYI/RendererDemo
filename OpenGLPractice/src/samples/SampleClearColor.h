#pragma once

#include "Sample.h"

namespace sample
{
	class SampleClearColor : public Sample
	{
	public:
		SampleClearColor();
		~SampleClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender(const Camera& camera, RenderScene* scenebuffer) override;
		void OnImGuiRenderer() override;

	private:
		float m_ClearColor[4];
	};
}
