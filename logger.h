#ifndef LOGGER_H
#define LOGGER_H
#define LOGGER(fmt) __FILE__, __LINE__, fmt

#include <string>

//日志级别枚举
typedef enum log_level {
  LOG_NOLEVEL,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
} log_level;

//设置日志级别，只能程序启动时设置
void set_logger_level(log_level level);

void logger_debug(const char *s_file, int s_line, std::string fmt, ...);

void logger_info(const char *s_file, int s_line, std::string fmt, ...);

void logger_warn(const char *s_file, int s_line, std::string fmt, ...);

void logger_error(const char *s_file, int s_line, std::string fmt, ...);

#endif  // LOGGER_H
