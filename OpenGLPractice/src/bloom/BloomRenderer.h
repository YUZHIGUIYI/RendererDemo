#pragma once

#include "BloomFBO.h"
#include "Shader.h"
#include <memory>

class BloomRenderer
{
public:
	BloomRenderer();
	~BloomRenderer();
	bool Init(unsigned int windowWidth, unsigned int windowHeight);
	void Destroy();
	void RenderBloomTexture(unsigned int srcTexture, float filterRadius);
	unsigned int BloomTexture();
	unsigned int BloomMip_i(int index);

	void RenderQuad();

private:
	void RenderDownsamples(unsigned int srcTexture);
	void RenderUpsamples(float filterRadius);

	bool mInit;
	std::unique_ptr<BloomFBO> mFBO;
	glm::ivec2 mSrcViewportSize;
	glm::vec2 mSrcViewportSizeFloat;
	std::unique_ptr<Renderer::Shader> mDownsampleShader;
	std::unique_ptr<Renderer::Shader> mUpsampleShader;

	bool mKarisAverageOnDownsample = true;

	unsigned int quadVAO, quadVBO;
};
