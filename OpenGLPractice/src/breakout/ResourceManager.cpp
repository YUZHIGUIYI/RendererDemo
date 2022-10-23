#include "ResourceManager.h"
#include <glad/glad.h>
#include <stb_image/stb_image.h>

namespace breakout
{

	std::unordered_map<std::string, std::unique_ptr<Renderer::Shader>> ResourceManager::Shaders;

	std::unordered_map<std::string, std::unique_ptr<Renderer::Texture2D>> ResourceManager::Textures;

	void ResourceManager::LoadShader(const std::string& vShaderFile, 
		const std::string& fShaderFile, const std::string& gShaderFile, const std::string& name)
	{
		//Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
		Shaders[name] = std::make_unique<Renderer::Shader>(vShaderFile, fShaderFile, gShaderFile);
	}

	Renderer::Shader* ResourceManager::GetShader(const std::string& name)
	{
		return Shaders[name].get();
	}

	void ResourceManager::LoadTexture(const std::string& file, bool alpha, 
		const std::string& name)
	{
		Textures[name] = std::make_unique<Renderer::Texture2D>();

		if (alpha)
		{
			Textures[name]->Internal_Format = GL_RGBA;
			Textures[name]->Image_Format = GL_RGBA;
		}
		// load image
		int width, height, nrChannels;
		unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
		// now generate texture
		Textures[name]->Generate(width, height, data);
		// and finally free image data
		stbi_image_free(data);
	}

	Renderer::Texture2D* ResourceManager::GetTexture(const std::string& name)
	{
		return Textures[name].get();
	}

	void ResourceManager::Clear()
	{
		for (const auto& iter : Shaders)
		{
			// TODO: Fix Me
			// 已有析构函数
			glDeleteProgram(iter.second->GetRendererID());
		}
		for (const auto& iter : Textures)
		{
			// TODO: Fix Me
			// 已有析构函数
			glDeleteTextures(1, &iter.second->ID);
		}
	}

	Renderer::Shader ResourceManager::loadShaderFromFile(const std::string& vShaderFile, const std::string& fShaderFile, 
		const std::string& gShaderFile)
	{
		return Renderer::Shader(vShaderFile, fShaderFile, gShaderFile);
	}

	Renderer::Texture2D ResourceManager::loadTextureFromFile(const std::string& file, bool alpha)
	{
		Renderer::Texture2D texture;

		if (alpha)
		{
			texture.Internal_Format = GL_RGBA;
			texture.Image_Format = GL_RGBA;
		}
		// load image
		int width, height, nrChannels;
		unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
		// now generate texture
		texture.Generate(width, height, data);
		// and finally free image data
		stbi_image_free(data);
		return texture;
	}

}