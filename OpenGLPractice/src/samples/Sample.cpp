#include "Sample.h"
//#include <imgui/imgui.h>

//namespace sample
//{
//
//	SampleMenu::SampleMenu(Sample*& currentSamplePointer)
//		: m_CurrentSample(currentSamplePointer)
//	{
//
//	}
//
//	SampleMenu::~SampleMenu()
//	{
//
//	}
//
//	void SampleMenu::OnImGuiRenderer()
//	{
//		for (auto& sam : m_Samples)
//		{
//			// �����ť������ʵ��
//			if (ImGui::Button(sam.first.c_str()))
//			{
//				m_CurrentSample = sam.second();
//				RD_INFO("Creating sample {0}", sam.first);
//			}
//		}
//	}
//
//}