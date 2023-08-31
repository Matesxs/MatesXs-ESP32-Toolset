#pragma once

#include "Arduino.h"

void asyncDelay(uint64_t t, void(*callback)());
void asyncDelayTerminate();
