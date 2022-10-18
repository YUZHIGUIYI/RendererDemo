#include "SampleMenu.h"
#include <imgui/imgui.h>

SampleMenu::SampleMenu(/*sample::Sample*& currentSamplePointer*/)
	: m_CurrentSample(/*currentSamplePointer*/ nullptr)
{
}

SampleMenu::~SampleMenu()
{
}

void SampleMenu::Update(const Camera& camera, RenderScene* scenebuffer)
{
	if (bshow)
	{
		if (!Begin("Debug Sample"))
		{
			End();
		}
		else 
		{
			if (m_CurrentSample != nullptr)
			{
				if (ImGui::Button("<-"))
				{
					m_CurrentSample.reset();
					RD_WARN("Delete sample {0}", m_CurrentSampleName);
				}
				if (m_CurrentSample != nullptr)
				{
					m_CurrentSample->OnRender(camera, scenebuffer);
					m_CurrentSample->OnImGuiRenderer();
				}
			}
			else
			{
				for (auto& sam : m_Samples)
				{
					// 点击按钮，创建实例
					if (ImGui::Button(sam.first.c_str()))
					{
						m_CurrentSample = sam.second();
						m_CurrentSampleName = sam.first;
						RD_INFO("Creating sample {0}", sam.first);
					}
				}
			}
			End();
		}
	}
}
