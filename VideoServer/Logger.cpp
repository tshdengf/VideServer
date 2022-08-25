#include "Logger.h"

std::shared_ptr<Logger> Logger::m_instance_ptr = nullptr;
std::mutex Logger::m_mutex;

std::shared_ptr<Logger> Logger::getInstance()
{
	if (m_instance_ptr == nullptr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_instance_ptr == nullptr)
		{
			m_instance_ptr = std::shared_ptr<Logger>(new Logger);
		}
	}
	return m_instance_ptr;
}

std::shared_ptr<spdlog::logger> Logger::getLogger()
{
	return m_logger;
}

Logger::Logger()
{
	const std::string log_dir = "./log";
	const std::string logger_name_prefix = "";

	bool console = false;

	std::string level = "trace";

	try
	{
		// logger name with timestamp
		int date = NowDateToInt();
		int time = NowTimeToInt();
		const std::string logger_name = logger_name_prefix + std::to_string(date);

		if (console)
			m_logger = spdlog::stdout_color_st(logger_name); // single thread console output faster
		else
			m_logger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(logger_name, log_dir + "/" + logger_name + ".log", 500 * 1024 * 1024, 1000); // multi part log files, with every part 500M, max 1000 files

		// custom format
		m_logger->set_pattern("%Y-%m-%d %H:%M:%S.%f <thread %t> [%l] [%@] %v"); // with timestamp, thread_id, filename and line number

		if (level == "trace")
		{
			m_logger->set_level(spdlog::level::trace);
			m_logger->flush_on(spdlog::level::trace);
		}
		else if (level == "debug")
		{
			m_logger->set_level(spdlog::level::debug);
			m_logger->flush_on(spdlog::level::debug);
		}
		else if (level == "info")
		{
			m_logger->set_level(spdlog::level::info);
			m_logger->flush_on(spdlog::level::info);
		}
		else if (level == "warn")
		{
			m_logger->set_level(spdlog::level::warn);
			m_logger->flush_on(spdlog::level::warn);
		}
		else if (level == "error")
		{
			m_logger->set_level(spdlog::level::err);
			m_logger->flush_on(spdlog::level::err);
		}
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
	}

}


