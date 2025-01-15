/**
 * @file log.h
 * @author David Long (david.long@snows.world)
 * @brief Logging utility for frostwm.
 * @version 0.1.0
 * @date 2025-01-04
 *
 * @copyright Copyright (c) 2025
 */

#ifndef _FROST_LOG_H
#define _FROST_LOG_H

/**
 * @brief Logs a message to stderr.
 *
 * Each log message is date-stamped and marked with the @ref log_level
 * specified. You can treat this almost like `fprintf`, where the first
 * argument is the severity of the message instead of the file descriptor.
 *
 * @param level The log level for the message.
 * @param fmt The format string for the message.
 * @param ... Additional arguments as specified in `fmt`.
 */
#define frost_log(level, fmt, ...)                                            \
  _frost_log (level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * @enum log_level
 * @brief Indicates the severity of a log message.
 */
enum log_level
{
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG,
};

/**
 * @brief INTERNAL USE ONLY.
 *
 * Please do not directly invoke this function. Instead, use @ref frost_log.
 *
 * Logs a message to `stderr`, complete with a timestamp, origin, line number,
 * and `printf`-like format capabilities.
 *
 * @param level The log severity as defined in @ref log_level.
 * @param file_loc The name of the file where this was called. Use `__FILE__`
 * here.
 * @param line The line number of where the function was called. Use `__LINE__`
 * here.
 * @param fmt The format string to print.
 * @param ... Additional arguments to supply the format string.
 *
 * @internal
 */
void _frost_log (enum log_level level, const char *file_loc, unsigned int line,
                 const char *fmt, ...);

#endif
