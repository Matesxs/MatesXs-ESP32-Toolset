#include "utils.h"

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

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
