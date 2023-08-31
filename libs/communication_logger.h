#pragma once

#include <Arduino.h>

#include "custom_queue.h"

template<size_t historySize>
class CommunicationLogger
{
public:
  CommunicationLogger(Stream& stream) :
    m_stream(stream)
  {}

  int available() const { return m_stream.available(); }

  String readString()
  {
    String string = m_stream.readString();
    processIncomingString(string);
    return string;
  }

  String readStringUntil(char terminator)
  {
    String string = m_stream.readStringUntil(terminator);
    processIncomingString(string);
    return string;
  }

  size_t printf(const char *format, ...)
  {
    char loc_buf[64];
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
      return 0;
    }
    if (len >= (int)sizeof(loc_buf))
    {
      temp = (char *)malloc(len + 1);
      if (temp == NULL)
      {
        va_end(arg);
        return 0;
      }
      len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    
    len = m_stream.write((uint8_t *)temp, len);
    processOutcomingString({ temp, static_cast<unsigned int>(len) });

    if (temp != loc_buf)
    {
      free(temp);
    }
    return len;
  }

  size_t print(String &s)
  {
    processOutcomingString(s);
    return m_stream.print(s);
  }

  size_t print(const char str[])
  {
    processOutcomingString(str);
    return m_stream.print(str);
  }

  size_t print(char c)
  {
    processOutcomingString(c);
    return m_stream.print(c);
  }

  size_t println(String &s)
  {
    processOutcomingString(s);
    return m_stream.println(s);
  }

  size_t println(const char str[])
  {
    processOutcomingString(str);
    return m_stream.println(str);
  }

  size_t println(char c)
  {
    processOutcomingString(c);
    return m_stream.println(c);
  }

  void flush()
  {
    m_stream.flush();
  }

  size_t HistorySize() const { return m_history.Size(); }
  bool HistoryEmpty() const { return m_history.IsEmpty(); }
  String HistoryGet() 
  { 
    String string;
    m_history.Deque(&string);
    return string;
  }
  void HistoryPop() { m_history.Deque(); }

private:
  void processIncomingString(String string)
  {
    string.trim();
    m_history.Enque("IN: " + string);
  }

  void processOutcomingString(String string)
  {
    string.trim();
    m_history.Enque("OUT: " + string);
  } 

private:
  Stream& m_stream;
  RotaryQueue<String, historySize> m_history;
};
