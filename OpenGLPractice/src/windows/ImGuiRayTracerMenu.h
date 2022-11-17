#pragma once

#include "ImGuiWindowBase.h"
#include "raytracing/RayTracing.h"

class ImGuiRayTracerMenu : public ImGuiWindowBase
{
public:
	ImGuiRayTracerMenu();
	~ImGuiRayTracerMenu();

	void Update(const Camera& camera, RenderScene* scenebuffer) override;

private:
	std::unique_ptr<RayTracer::RayTracing> rayTracingSample;

};