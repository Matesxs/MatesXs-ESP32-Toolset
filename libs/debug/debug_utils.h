#pragma once

#include <Arduino.h>

namespace DebugUtils
{
  String format_string(const char *format, ...);
  String format_string(String format, ...);
  String stripString(String string);
}