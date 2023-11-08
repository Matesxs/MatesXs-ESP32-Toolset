#pragma once

#include "debug_utils.h"

#if DEBUG_INFO_LEVEL == 2
#undef DEBUG
#define DEBUG(...) Serial.printf("%s at %d\t%s\r\n", __FILE__, __LINE__, DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#else
#if DEBUG_INFO_LEVEL == 1
#define DEBUG(...) Serial.printf("%s\r\n", DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#else
#define DEBUG(...)
#endif
#endif
