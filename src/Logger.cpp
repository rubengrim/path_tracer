#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

bool Logger::Initialize()
{
	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%H:%M:%S.%e] %v%$");

	spdlog::sinks_init_list sinkList = { consoleSink };
		
	std::shared_ptr<spdlog::logger> pulseLogger = std::make_shared<spdlog::logger>("Pulse", sinkList.begin(), sinkList.end());
	pulseLogger->set_level(spdlog::level::trace);
	pulseLogger->flush_on(spdlog::level::trace);
	spdlog::register_logger(pulseLogger);

	return true;
}

void Logger::Shutdown()
{
	spdlog::shutdown();
}