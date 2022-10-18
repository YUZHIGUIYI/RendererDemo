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

	// 注册测试类
	template<class T>
	void RegisterSample(const std::string& name)
	{
		RD_INFO("Register sample {0}", name);
		m_Samples.push_back(std::make_pair(name, []() { /*return new T()*/ return std::make_unique<T>(); }));
	}
	
private:
	// 当前测试实例的名称
	std::string m_CurrentSampleName;
	// 指向当前测试实例的指针
	std::unique_ptr<sample::Sample> m_CurrentSample;
	std::vector<std::pair<std::string, std::function</*sample::Sample* ()*/ std::unique_ptr<sample::Sample> ()>>> m_Samples;
};
