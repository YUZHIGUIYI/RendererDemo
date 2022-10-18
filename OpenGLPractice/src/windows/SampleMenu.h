#pragma once

#include "samples/Sample.h"
#include "ImGuiWindowBase.h"
#include "Log.h"
#include <vector>
#include <functional>
#include <memory>

class SampleMenu : public ImGuiWindowBase
{
public:
	SampleMenu(/*sample::Sample*& currentSamplePointer*/);
	~SampleMenu();

	void Update(const Camera& camera, RenderScene* scenebuffer) override;

	// ע�������
	template<class T>
	void RegisterSample(const std::string& name)
	{
		RD_INFO("Register sample {0}", name);
		m_Samples.push_back(std::make_pair(name, []() { /*return new T()*/ return std::make_unique<T>(); }));
	}
	
private:
	// ��ǰ����ʵ��������
	std::string m_CurrentSampleName;
	// ָ��ǰ����ʵ����ָ��
	std::unique_ptr<sample::Sample> m_CurrentSample;
	std::vector<std::pair<std::string, std::function</*sample::Sample* ()*/ std::unique_ptr<sample::Sample> ()>>> m_Samples;
};
