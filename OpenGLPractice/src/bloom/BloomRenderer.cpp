#include "BloomRenderer.h"
#include "Log.h"

BloomRenderer::BloomRenderer() : mInit(false), quadVAO(0), quadVBO(0) {}

BloomRenderer::~BloomRenderer() {}

bool BloomRenderer::Init(unsigned int windowWidth, unsigned int windowHeight)
{
	if (mInit) return true;

	// BloomFBO
	mFBO = std::make_unique<BloomFBO>();

	mSrcViewportSize = glm::ivec2(windowWidth, windowHeight);
	mSrcViewportSizeFloat = glm::vec2((float)windowWidth, (float)windowHeight);

	// FrameBuffer
	// #TODO: Play around with this value
	const unsigned int num_bloom_mips = 6;
	bool status = mFBO->Init(windowWidth, windowHeight, num_bloom_mips);
	if (!status)
	{
		RD_ERROR("Failed to initialize bloom FBO - cannot create bloom renderer!");
		return false;
	}

	// shaders
	mDownsampleShader = std::make_unique<Renderer::Shader>("res/shaders/phybloom/NewDownSample.vert",
		"res/shaders/phybloom/NewDownSample.frag");
	mUpsampleShader = std::make_unique<Renderer::Shader>("res/shaders/phybloom/NewUpSample.vert",
		"res/shaders/phybloom/NewUpSample.frag");

	// downsample
	mDownsampleShader->Bind();
	mDownsampleShader->SetUniform1i("srcTexture", 0);
	mDownsampleShader->Unbind();

	// upsample
	mUpsampleShader->Bind();
	mUpsampleShader->SetUniform1i("srcTexture", 0);
	mUpsampleShader->Unbind();

	return true;
}

void BloomRenderer::Destroy()
{
	mFBO->Destroy();
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

void BloomRenderer::RenderBloomTexture(unsigned int srcTexture, float filterRadius)
{
	mFBO->BindForWriting();

	RenderDownsamples(srcTexture);
	RenderUpsamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// restore viewport
	glViewport(0, 0, mSrcViewportSize.x, mSrcViewportSize.y);
}

unsigned int BloomRenderer::BloomTexture()
{
	return mFBO->MipChain()[0].texture;
}

unsigned int BloomRenderer::BloomMip_i(int index)
{
	const auto& mipChain = mFBO->MipChain();
	int size = (int)mipChain.size();
	return mipChain[(index > size - 1) ? size - 1 : (index < 0) ? 0 : index].texture;
}

void BloomRenderer::RenderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void BloomRenderer::RenderDownsamples(unsigned int srcTexture)
{
	const auto& mipChain = mFBO->MipChain();

	mDownsampleShader->Bind();
	mDownsampleShader->SetUniformVec2f("srcResolution", mSrcViewportSizeFloat);
	if (mKarisAverageOnDownsample) mDownsampleShader->SetUniform1i("mipLevel", 0);

	// bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	// progressively downsample through the mip chain
	for (int i = 0; i < mipChain.size(); ++i)
	{
		const BloomMip& mip = mipChain[i];
		glViewport(0, 0, mip.size.x, mip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, mip.texture, 0);

		// render screen-filled quad of resolution of current mip
		RenderQuad();

		// set current mip resolution as srcResolution for next iteration
		mDownsampleShader->SetUniformVec2f("srcResolution", mip.size);
		// set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// disable Karis average for consequent downsamples
		if (i == 0) mDownsampleShader->SetUniform1i("mipLevel", 1);
	}
	mDownsampleShader->Unbind();
}

void BloomRenderer::RenderUpsamples(float filterRadius)
{
	const auto& mipChain = mFBO->MipChain();

	mUpsampleShader->Bind();
	mUpsampleShader->SetUniform1f("filterRadius", filterRadius);

	// enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = mipChain.size() - 1; i > 0; --i)
	{
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		// bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// set framebuffer render target (we write to this texture)
		glViewport(0, 0, nextMip.size.x, nextMip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, nextMip.texture, 0);

		// render screen-filled quad of resolution of current mip
		RenderQuad();
	}

	// disable additive blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	mUpsampleShader->Unbind();
}