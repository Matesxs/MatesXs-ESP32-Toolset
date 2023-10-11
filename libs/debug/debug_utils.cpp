#include "debug_utils.h"

namespace DebugUtils
{
  String format_string(const char *format, ...)
  {
    char loc_buf[32];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;

    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);

    if (len < 0)
    {
      va_end(arg);
      return String();
    };

    if (len >= sizeof(loc_buf))
    {
      temp = (char *)malloc(len + 1);
      if (temp == NULL)
      {
        va_end(arg);
        return String();
      }

      len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);

    String outputBuffer(temp);

    if (temp != loc_buf)
      free(temp);

    return outputBuffer;
  }

  String format_string(String format, ...)
  {
    va_list arg;
    va_start(arg, format);
    String ret = format_string(format.c_str(), arg);
    va_end(arg);
    return ret;
  }

  String stripString(String string)
  {
    string.trim();
    return string;
  }
}
