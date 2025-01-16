#include "frostwm/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define MAX_TIMESTR_SIZE 20

#define COLOR_RESET "\033[0m"
#define COLOR_ERROR "\033[31m"
#define COLOR_WARN  "\033[33m"
#define COLOR_INFO  "\033[34m"
#define COLOR_DEBUG "\033[90m"

void
_frost_log (enum log_level level, const char *file_loc, unsigned int line,
            const char *fmt, ...)
{
  const char *color;
  const char *msg_color;
  const char *level_str;
  time_t      now;
  struct tm  *local_time;
  char        time_str[MAX_TIMESTR_SIZE];
  va_list     args;

  switch (level)
    {
    case LOG_ERROR:
      color     = COLOR_ERROR;
      msg_color = COLOR_ERROR;
      level_str = "ERROR";
      break;
    case LOG_WARN:
      color     = COLOR_WARN;
      msg_color = COLOR_WARN;
      level_str = "WARN ";
      break;
    case LOG_INFO:
      color     = COLOR_INFO;
      msg_color = COLOR_RESET;
      level_str = "INFO ";
      break;
    case LOG_DEBUG:
      color     = COLOR_DEBUG;
      msg_color = COLOR_RESET;
      level_str = "DEBUG";
      break;
    }

  now        = time (NULL);
  local_time = localtime (&now);
  strftime (time_str, MAX_TIMESTR_SIZE, "%Y-%m-%d %H:%M:%S", local_time);

  va_start (args, fmt);
  fprintf (stderr, COLOR_DEBUG "%s %s%s%s [%s:%u] %s", time_str, color,
           level_str, COLOR_DEBUG, file_loc, line, msg_color);
  vfprintf (stderr, fmt, args);
  fprintf (stderr, COLOR_RESET "\n");
  va_end (args);
}