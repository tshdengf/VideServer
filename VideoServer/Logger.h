#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <time.h>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h> 
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>


static inline int NowDateToInt()
{
	time_t now;
	time(&now);
	tm p;
	localtime_r(&now, &p);
	int now_date = (1900 + p.tm_year) * 10000 + (p.tm_mon + 1) * 100 + p.tm_mday;
	return now_date;
}

static inline int NowTimeToInt()
{
	time_t now;
	time(&now);
	tm p;
	localtime_r(&now, &p);
	int now_int = p.tm_hour * 10000 + p.tm_min * 100 + p.tm_sec;
	return now_int;
}

class Logger
{
public:
	static std::shared_ptr<Logger> getInstance();

	std::shared_ptr<spdlog::logger> getLogger();

private:
	Logger();


	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
	std::shared_ptr<spdlog::logger> m_logger;
	static std::mutex m_mutex;
	static std::shared_ptr<Logger> m_instance_ptr;
};

#define TRACE(...) SPDLOG_LOGGER_CALL(Logger::getInstance().get()->getLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define DEBUG(...) SPDLOG_LOGGER_CALL(Logger::getInstance().get()->getLogger().get(), spdlog::level::debug, __VA_ARGS__)
#define INFO(...) SPDLOG_LOGGER_CALL(Logger::getInstance().get()->getLogger().get(), spdlog::level::info, __VA_ARGS__)
#define WARN(...) SPDLOG_LOGGER_CALL(Logger::getInstance().get()->getLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define ERROR(...) SPDLOG_LOGGER_CALL(Logger::getInstance().get()->getLogger().get(), spdlog::level::err, __VA_ARGS__)
