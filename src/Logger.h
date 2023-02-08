#pragma once
#include "spdlog/spdlog.h"
#include <memory>

class Logger
{
public:
	bool Initialize();
	void Shutdown();
};

// spdlog macros

#define PULSE_DEBUG
#ifdef PULSE_DEBUG
#define PULSE_LOGGER_NAME	"Pulse"
#define PULSE_TRACE(...)	if (spdlog::get(PULSE_LOGGER_NAME) != nullptr) { spdlog::get(PULSE_LOGGER_NAME)->trace(__VA_ARGS__); }
#define PULSE_INFO(...)		if (spdlog::get(PULSE_LOGGER_NAME) != nullptr) { spdlog::get(PULSE_LOGGER_NAME)->info(__VA_ARGS__); }
#define PULSE_DEBUG(...)	if (spdlog::get(PULSE_LOGGER_NAME) != nullptr) { spdlog::get(PULSE_LOGGER_NAME)->debug(__VA_ARGS__); }
#define PULSE_ERROR(...)	if (spdlog::get(PULSE_LOGGER_NAME) != nullptr) { spdlog::get(PULSE_LOGGER_NAME)->error(__VA_ARGS__); }
#define PULSE_WARN(...)	    if (spdlog::get(PULSE_LOGGER_NAME) != nullptr) { spdlog::get(PULSE_LOGGER_NAME)->warn(__VA_ARGS__); }
#else
#define PULSE_TRACE(...)	(void)0;
#define PULSE_INFO(...)		(void)0;
#define PULSE_DEBUG(...)	(void)0;
#define PULSE_ERROR(...)	(void)0;
#define PULSE_WARN(...)	    (void)0;
#endif
