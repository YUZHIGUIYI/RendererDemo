#include "windows/RenderWindow.h"
#include "windows/ImGuiUIManager.h"
#include "windows/SampleMenu.h"
//#include "samples/SampleClearColor.h"
#include "samples/SampleRenderModel.h"
#include "samples/SampleStencil.h"
#include "samples/SampleBlending.h"
#include "samples/SampleBlendingWindow.h"
#include "samples/SampleFaceCulling.h"
#include "samples/SampleFrameBuffers.h"
//#include "samples/SampleSkyBox.h"
#include "samples/SampleSkyBoxNanosuit.h"
//#include "samples/SampleAdvancedGLSL.h"
//#include "samples/SampleGeometryHouse.h"
#include "samples/SampleGeometryExplode.h"
#include "samples/SampleGeometryNormal.h"
//#include "samples/SampleInstancing.h"
#include "samples/SampleAsteroids.h"
#include "samples/SampleMSAA.h"
//#include "samples/SampleBlinnPhong.h"
//#include "samples/SampleGammaCorrection.h"
#include "samples/SampleShadowMapping.h"
#include "samples/SamplePointShadows.h"
#include "samples/SampleNormalMapping.h"
#include "samples/SampleParallaxMapping.h"
#include "samples/SampleHDR.h"
#include "samples/SampleBloom.h"
#include "samples/SampleDeferredShading.h"
#include "samples/SampleSSAO.h"
#include "samples/SampleLightingTextured.h"
#include "samples/SampleIBLIrradiance.h"
#include "samples/SampleIBLSpecular.h"
#include "samples/SampleIBLSpecularTexture.h"
#include "samples/SampleTextRendering.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <irrKlang/irrKlang.h>

#include "Log.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main()
{
	Renderer::Log::Init();
	RD_CORE_INFO("Welcome to Renderer Demo");

	// Create render window
	RenderWindow renderwindow(WIDTH, HEIGHT, "Renderer Demo");

	// Enable OpenGL Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// ImGui UI Manager
	ImGuiUIManager UIManager(renderwindow.GetGLFWwindow());

	// Add UI component
	std::unique_ptr<SampleMenu> sampleMenu = std::make_unique<SampleMenu>();
	//sampleMenu->RegisterSample<sample::SampleClearColor>("Clear Color");
	sampleMenu->RegisterSample<sample::SampleRenderModel>("Render Nano-suit");
	sampleMenu->RegisterSample<sample::SampleStencil>("Stencil Testing");
	sampleMenu->RegisterSample<sample::SampleBlending>("Blending Grass");
	sampleMenu->RegisterSample<sample::SampleBlendingWindow>("Blending Window");
	sampleMenu->RegisterSample<sample::SampleFaceCulling>("Face Culling");
	sampleMenu->RegisterSample<sample::SampleFrameBuffers>("Frame Buffer");
	//sampleMenu->RegisterSample<sample::SampleSkyBox>("Sky Box");
	sampleMenu->RegisterSample<sample::SampleSkyBoxNanosuit>("Sky Box Nano-suit");
	//sampleMenu->RegisterSample<sample::SampleAdvancedGLSL>("Advanced GLSL");
	//sampleMenu->RegisterSample<sample::SampleGeometryHouse>("Geometry House");
	sampleMenu->RegisterSample<sample::SampleGeometryExplode>("Geometry Explode");
	sampleMenu->RegisterSample<sample::SampleGeometryNormal>("Geometry Normal");
	//sampleMenu->RegisterSample<sample::SampleInstancing>("Instances");
	sampleMenu->RegisterSample<sample::SampleAsteroids>("Asteroids Instance");
	sampleMenu->RegisterSample<sample::SampleMSAA>("MSAA");
	//sampleMenu->RegisterSample<sample::SampleBlinnPhong>("BlinnPhong");
	//sampleMenu->RegisterSample<sample::SampleGammaCorrection>("Gamma Correction");
	sampleMenu->RegisterSample<sample::SampleShadowMapping>("Shadow Mapping");
	sampleMenu->RegisterSample<sample::SamplePointShadows>("Point Shadows");
	sampleMenu->RegisterSample<sample::SampleNormalMapping>("Normal Mapping");
	sampleMenu->RegisterSample<sample::SampleParallaxMapping>("Parallax Mapping");
	sampleMenu->RegisterSample<sample::SampleHDR>("High Dynamic Range");
	sampleMenu->RegisterSample<sample::SampleBloom>("Bloom with HDR");
	sampleMenu->RegisterSample<sample::SampleDeferredShading>("Deferred Shading");
	sampleMenu->RegisterSample<sample::SampleSSAO>("Screen-Space Ambient Occlusion");
	sampleMenu->RegisterSample<sample::SampleLightingTextured>("Physically Based Rendering");
	sampleMenu->RegisterSample<sample::SampleIBLIrradiance>("Diffuse Irradiance");
	sampleMenu->RegisterSample<sample::SampleIBLSpecular>("Specular IBL");
	sampleMenu->RegisterSample<sample::SampleIBLSpecularTexture>("Specular IBL Texture");
	sampleMenu->RegisterSample<sample::SampleTextRendering>("Text Rendering");

	UIManager.AddWindow(std::move(sampleMenu));

	// audio example
	irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
	SoundEngine->play2D("res/audio/breakout.mp3", true);

	// Render cycle
	while (!renderwindow.RenderWindowShouldClose())
	{
		renderwindow.ProcessInput();

		// Clear color buffer | depth buffer | stencil buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Start ImGui frame
		UIManager.Begin(RenderWindow::camera);

		// ImGui rendering
		UIManager.End();

		renderwindow.RenderWindowSwapBuffers();
		renderwindow.RenderWindowPollEvents();
	}

	SoundEngine->drop();

	return 0;
}