void hw_init();
void hw_close();
void update_buffers_in();
void update_buffers_out();

void update_time();
void timer_init();
void timer_sleep_until(unsigned long long delay_ns);

#ifdef HW_LINUX
#include <stdio.h>
#define logf printf
#endif

#ifdef HW_ESP32
#include <Arduino.h>
#define logf Serial.printf
#endif

#ifdef DEBUG
#define DEBUGF logf
#else
#define DEBUGF(...)
#endif