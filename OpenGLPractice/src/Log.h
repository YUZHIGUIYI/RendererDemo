#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include <memory>

namespace Renderer
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
// Core log ºê
#define RD_CORE_TRACE(...)		::Renderer::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RD_CORE_INFO(...)		::Renderer::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RD_CORE_WARN(...)		::Renderer::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RD_CORE_ERROR(...)		::Renderer::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RD_CORE_CRITICAL(...)	::Renderer::Log::GetCoreLogger()->critical(__VA_ARGS__)
// Client log ºê
#define RD_TRACE(...)			::Renderer::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RD_INFO(...)			::Renderer::Log::GetClientLogger()->info(__VA_ARGS__)
#define RD_WARN(...)			::Renderer::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RD_ERROR(...)			::Renderer::Log::GetClientLogger()->error(__VA_ARGS__)
#define RD_CRITICAL(...)		::Renderer::Log::GetClientLogger()->critical(__VA_ARGS__)
