#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::consoleLogger;

bool Logger::Initialize()
{
	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%H:%M:%S.%e] %v%$");

	spdlog::sinks_init_list sinkList = { consoleSink };
		
	consoleLogger = std::make_shared<spdlog::logger>("ConsoleLogger", sinkList.begin(), sinkList.end());
	consoleLogger->set_level(spdlog::level::trace);
	consoleLogger->flush_on(spdlog::level::trace);
	spdlog::register_logger(consoleLogger);

	return true;
}

void Logger::Shutdown()
{
	spdlog::shutdown();
}
