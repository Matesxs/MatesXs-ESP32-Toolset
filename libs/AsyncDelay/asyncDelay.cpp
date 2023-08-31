#include "asyncDelay.h"

#define CRITICAL(X, Y)       \
  portENTER_CRITICAL_ISR(Y); \
  X;                         \
  portEXIT_CRITICAL_ISR(Y);

typedef struct
{
  uint64_t count = 0;
  uint64_t target = 0;
  bool isRunning = false;

  void (*callback)() = nullptr;

  hw_timer_t *timer = NULL;
} DelayHolder;

volatile DelayHolder asyncDelInstance;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void onTimeTask(void *)
{
  asyncDelInstance.callback();
  asyncDelInstance.isRunning = false;

  vTaskDelete(NULL);
}

void IRAM_ATTR onTime()
{
  CRITICAL(asyncDelInstance.count++, &timerMux);

  if (asyncDelInstance.count >= asyncDelInstance.target)
  {
    timerAlarmDisable(asyncDelInstance.timer);
    timerDetachInterrupt(asyncDelInstance.timer);
    timerEnd(asyncDelInstance.timer);

    asyncDelInstance.count = 0;
    asyncDelInstance.timer = nullptr;

    if (asyncDelInstance.callback != nullptr)
      xTaskCreatePinnedToCore(onTimeTask, "ad_on_time", 4096, NULL, 1, NULL, 1);
    else
      asyncDelInstance.isRunning = false;
  }
}

void asyncDelay(uint64_t t, void(*callback)())
{
  if (asyncDelInstance.isRunning) return;

  portENTER_CRITICAL_ISR(&timerMux);
  if (asyncDelInstance.timer != nullptr) 
  {
    portEXIT_CRITICAL_ISR(&timerMux);
    return;
  }

  asyncDelInstance.count = 0;
  asyncDelInstance.target = t;
  asyncDelInstance.callback = callback;

  asyncDelInstance.timer = timerBegin(2, 80, true);
  timerAttachInterrupt(asyncDelInstance.timer, &onTime, true);
  timerAlarmWrite(asyncDelInstance.timer, 1000, true); 
  timerAlarmEnable(asyncDelInstance.timer);
  asyncDelInstance.isRunning = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void asyncDelayTerminate()
{
  if (asyncDelInstance.timer != nullptr)
  {
    timerAlarmDisable(asyncDelInstance.timer);
    timerDetachInterrupt(asyncDelInstance.timer);
    timerEnd(asyncDelInstance.timer);
  }

  asyncDelInstance.count = 0;
  asyncDelInstance.timer = nullptr;
  asyncDelInstance.isRunning = false;
}
