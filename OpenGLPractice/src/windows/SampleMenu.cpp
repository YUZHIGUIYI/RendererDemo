#include "SampleMenu.h"
#include <imgui/imgui.h>
#include "samples/SamplePCH.h"

SampleMenu::SampleMenu(/*sample::Sample*& currentSamplePointer*/)
	: m_CurrentSample(/*currentSamplePointer*/ nullptr)
{
	RegisterSample<sample::SampleRenderModel>("Render Nano-suit");
	RegisterSample<sample::SampleStencil>("Stencil Testing");
	RegisterSample<sample::SampleBlending>("Blending Grass");
	RegisterSample<sample::SampleBlendingWindow>("Blending Window");
	RegisterSample<sample::SampleFaceCulling>("Face Culling");
	RegisterSample<sample::SampleSkyBoxNanosuit>("Sky Box Nano-suit");
	//RegisterSample<sample::SampleGeometryHouse>("Geometry House");
	RegisterSample<sample::SampleGeometryExplode>("Geometry Explode");
	RegisterSample<sample::SampleGeometryNormal>("Geometry Normal");
	//RegisterSample<sample::SampleInstancing>("Instances");
	RegisterSample<sample::SampleAsteroids>("Asteroids Instance");
	RegisterSample<sample::SampleMSAA>("MSAA");
	//RegisterSample<sample::SampleBlinnPhong>("BlinnPhong");
	//RegisterSample<sample::SampleGammaCorrection>("Gamma Correction");
	RegisterSample<sample::SampleShadowMapping>("Shadow Mapping");
	RegisterSample<sample::SamplePointShadows>("Point Shadows");
	RegisterSample<sample::SampleNormalMapping>("Normal Mapping");
	RegisterSample<sample::SampleParallaxMapping>("Parallax Mapping");
	RegisterSample<sample::SampleHDR>("High Dynamic Range");
	RegisterSample<sample::SampleBloom>("Bloom with HDR");
	RegisterSample<sample::SampleDeferredShading>("Deferred Shading");
	RegisterSample<sample::SampleSSAO>("Screen-Space Ambient Occlusion");
	RegisterSample<sample::SampleLightingTextured>("Physically Based Rendering");
	RegisterSample<sample::SampleIBLIrradiance>("Diffuse Irradiance");
	RegisterSample<sample::SampleIBLSpecular>("Specular IBL");
	RegisterSample<sample::SampleIBLSpecularTexture>("Specular IBL Texture");
	RegisterSample<sample::SampleTextRendering>("Text Rendering");
	RegisterSample<sample::SampleWeightedOIT>("Order-Independent transparency");
	RegisterSample<sample::SampleSkeletalAnimation>("Skeletal Animation");
	RegisterSample<sample::SampleCSM>("Cascaded Shadow Mapping");
	RegisterSample<sample::SampleFrustumCulling>("Frustum Culling via BVH");
	RegisterSample<sample::SampleRenderTerrainCPU>("Rendering Terrain via CPU");
	RegisterSample<sample::SampleRenderTerrainGPU>("Rendering Terrain via Tessellation Shader");
	RegisterSample<sample::SamplePhysicallyBasedBloom>("Physically Based Bloom");
	RegisterSample<sample::SampleComputeShader>("Compute Shader");
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
