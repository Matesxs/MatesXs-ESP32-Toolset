#pragma once

#if DEBUG_LEVEL != 0

#include "debug_utils.h"

#if DEBUG_LEVEL >= 3
#ifdef DEBUG_LOCATION
#define DEBUG(...) if (Serial) Serial.printf("[DEBUG] %s at %d\t%s\r\n", __FILE__, __LINE__, DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#else
#define DEBUG(...) if (Serial) Serial.printf("[DEBUG] %s\r\n", DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#endif
#else
#define DEBUG(...)
#endif

#if DEBUG_LEVEL >= 2
#ifdef DEBUG_LOCATION
#define INFO(...) if (Serial) Serial.printf("[INFO] %s at %d\t%s\r\n", __FILE__, __LINE__, DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#else
#define INFO(...) if (Serial) Serial.printf("[INFO] %s\r\n", DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#endif
#else
#define INFO(...)
#endif

#if DEBUG_LEVEL >= 1
#ifdef DEBUG_LOCATION
#define ERROR(...) if (Serial) Serial.printf("[ERROR] %s at %d\t%s\r\n", __FILE__, __LINE__, DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#else
#define ERROR(...) if (Serial) Serial.printf("[ERROR] %s\r\n", DebugUtils::stripString(DebugUtils::format_string(__VA_ARGS__)).c_str())
#endif
#else
#define ERROR(...)
#endif

#else
#define DEBUG(...)
#define INFO(...)
#define ERROR(...)
#endif