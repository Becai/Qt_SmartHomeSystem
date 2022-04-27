//
// Created by Hami Lemon on 2022/4/24.
//

#include "logger.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

using namespace std;
//日志格式最大长度
#define LOGGER_FMT_LENGTH 256
#define BUFFER_SIZE 1024                       //日志信息缓冲区大小
static log_level current_level = LOG_NOLEVEL;  //当前日志级别
/**
 * 设置日志格式
 * %%: 转义%
 * %f: 文件名
 * %n: 行号
 * %m: 日志信息
 * %l: 日志级别
 */
string logger_fmt = "%l %f:%n %m";  //日志信息格式

const string LOGGER_LEVEL_TABLE[] = {"[DEFAULT]", "[DEBUG]", "[INFO]", "[WARN]",
                                     "[ERROR]"};

//设置日志级别，只能程序启动时设置
void set_logger_level(log_level level) {
  if (current_level == LOG_NOLEVEL) {
    current_level = level;
  }
}

//获取文件名
const char *logger_basename(const char *path) {
  const char *slash = strrchr(path, '/');
  if (slash) {
    path = slash + 1;
  }
  return path;
}

//格式化日志信息
string logger_format(const char *s_file, int s_line, log_level level,
                     string msg) {
  s_file = logger_basename(s_file);  //文件名

  string result(logger_fmt);
  int pos = result.find("%%");
  if (pos >= 0) {
    result.replace(pos, 2, "%");
  }

  pos = result.find("%f");
  if (pos >= 0) {
    result.replace(pos, 2, s_file);
  }

  pos = result.find("%n");
  if (pos >= 0) {
    result.replace(pos, 2, to_string(s_line));
  }

  pos = result.find("%l");
  if (pos >= 0) {
    result.replace(pos, 2, LOGGER_LEVEL_TABLE[level]);
  }

  pos = result.find("%m");
  if (pos >= 0) {
    result.replace(pos, 2, msg);
  }
  return result;
}

void logger_log(const char *s_file, int s_line, log_level level, string fmt,
                va_list ap) {
  char buffer[BUFFER_SIZE];
  int msg_len = vsnprintf(buffer, BUFFER_SIZE, fmt.c_str(), ap);
  if ((size_t)msg_len >= BUFFER_SIZE) {
    //日志信息过长
    fprintf(stderr, "log msg to long!\n");
    return;
  }
  string msg = logger_format(s_file, s_line, level, buffer);

  if (level == LOG_ERROR) {
    // error级别输出到错误流中
    cerr << msg << endl;
  } else {
    cout << msg << endl;
  }
}

void logger_debug(const char *s_file, int s_line, string fmt, ...) {
  if (LOG_DEBUG < current_level) {
    return;
  }
  va_list ap;
  va_start(ap, fmt);
  logger_log(s_file, s_line, LOG_DEBUG, fmt, ap);
  va_end(ap);
}

void logger_info(const char *s_file, int s_line, string fmt, ...) {
  if (LOG_INFO < current_level) {
    return;
  }
  va_list ap;
  va_start(ap, fmt);
  logger_log(s_file, s_line, LOG_INFO, fmt, ap);
  va_end(ap);
}

void logger_warn(const char *s_file, int s_line, string fmt, ...) {
  if (LOG_WARN < current_level) {
    return;
  }
  va_list ap;
  va_start(ap, fmt);
  logger_log(s_file, s_line, LOG_WARN, fmt, ap);
  va_end(ap);
}

void logger_error(const char *s_file, int s_line, string fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  logger_log(s_file, s_line, LOG_ERROR, fmt, ap);
  va_end(ap);
}
