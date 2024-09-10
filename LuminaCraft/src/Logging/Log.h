#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Lumina
{

	class Log
	{
	public:

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }


	private:

		static std::shared_ptr<spdlog::logger> s_Logger;

	};
}

#define LOG_WARN(...) Lumina::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Lumina::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Lumina::Log::GetLogger()->critical(__VA_ARGS__)
#define LOG_TRACE(...) Lumina::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) Lumina::Log::GetLogger()->info(__VA_ARGS__)