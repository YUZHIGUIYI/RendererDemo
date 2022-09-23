#pragma once

#include <functional>
#include <vector>
#include "Camera.h"
#include "Log.h"

namespace sample
{
	class Sample
	{
	public:
		Sample() {}
		virtual ~Sample() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender(const Camera& camera) {}
		virtual void OnImGuiRenderer() {}
	};

	//class SampleMenu : public Sample
	//{
	//public:
	//	SampleMenu(Sample*& currentSamplePointer);
	//	~SampleMenu();

	//	void OnImGuiRenderer() override;

	//	// ע��
	//	template<class T>
	//	void RegisterSample(const std::string& name)
	//	{
	//		RD_INFO("Registering sample {0}", name);
	//		m_Samples.push_back(std::make_pair(name, []() {return new T(); }));
	//	}

	//private:
	//	// ָ��ǰ���Ե�ָ�������
	//	Sample*& m_CurrentSample;
	//	std::vector<std::pair<std::string, std::function<Sample* ()>>> m_Samples;
	//};
}