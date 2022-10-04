#pragma once

#include <unordered_map>
#include <string>

#include "Texture2D.h"
#include "Shader.h"

namespace breakout
{
	// A static singleton ResourceManager class that hosts several
	// functions to load Textures and Shaders. Each loaded texture
	// and/or shader is also stored for future reference by string
	// handles. All functions and resources are static and no 
	// public constructor is defined.
	class ResourceManager
	{
	public:
		// resource storage
		static std::unordered_map<std::string, Renderer::Shader> Shaders;
		static std::unordered_map<std::string, Renderer::Texture2D> Textures;
		// loads (and generates) a shader program from file loading vertex, fragment (and geometry)
		// shaders's source code.
		// if gShaderFile is not empty, it also loads a geometry shader
		static Renderer::Shader LoadShader(const std::string& vShaderFile, const std::string& fShaderFile,
			const std::string& gShaderFile, const std::string& name);
		// retrieves a stored sader
		static Renderer::Shader GetShader(const std::string& name);
		// loads (and generates) a texture from file
		static Renderer::Texture2D LoadTexture(const std::string& file, bool alpha, const std::string& name);
		// retrieves a stored texture
		static Renderer::Texture2D GetTexture(const std::string& name);
		// properly de-allocates all loaded resources
		static void Clear();

	private:
		ResourceManager() {}
		// loads and generates a shader from file
		static Renderer::Shader loadShaderFromFile(const std::string& vShaderFile, const std::string& fShaderFile,
			const std::string& gShaderFile);
		// loads a single texture from file
		static Renderer::Texture2D loadTextureFromFile(const std::string& file, bool alpha);
	};
}