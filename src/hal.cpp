#ifdef HW_LINUX
#endif

#include "iec_types.h"

#include "hal.h"

//Booleans
extern IEC_BOOL *bool_input[BUFFER_SIZE][8];
extern IEC_BOOL *bool_output[BUFFER_SIZE][8];

//Bytes
extern IEC_BYTE *byte_input[BUFFER_SIZE];
extern IEC_BYTE *byte_output[BUFFER_SIZE];

//Analog I/O
extern IEC_UINT *int_input[BUFFER_SIZE];
extern IEC_UINT *int_output[BUFFER_SIZE];

//Memory
extern IEC_UINT *int_memory[BUFFER_SIZE];
extern IEC_DINT *dint_memory[BUFFER_SIZE];
extern IEC_LINT *lint_memory[BUFFER_SIZE];

//Special Functions
extern IEC_LINT *special_functions[BUFFER_SIZE];

extern IEC_TIME __CURRENT_TIME;
extern unsigned long long common_ticktime__;

#define MILLION 1000000
#define BILLION 1000000000

void update_time()
{
    __CURRENT_TIME.tv_nsec += common_ticktime__;
    if (__CURRENT_TIME.tv_nsec >= BILLION)
    {
        __CURRENT_TIME.tv_nsec -= BILLION;
        __CURRENT_TIME.tv_sec++;
    }
}

/*
 * Macros black magic to debug print only variables which are really used in the PLC program.
 */

#define POOL_BOOL_I bool_input
#define POOL_BOOL_Q bool_output
#define POOL_UINT_I int_input
#define INDEX_BOOL(a, b) [a][b]
#define INDEX_UINT(a, b) [a]

#define print_in_Q(...)
inline void print_in_I(const char *name, IEC_BOOL *val)
{
    logf("%s = %u\n", name, *val);
}

void print_inputs()
{
#define __LOCATED_VAR(type, name, inout, type_sym, a, b) \
    print_in_##inout(#inout #type_sym #a "." #b, POOL_##type##_##inout INDEX_##type(a, b));
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR
}

#define print_out_I(...)
inline void print_out_Q(const char *name, IEC_BOOL *val)
{
    logf("%s = %u\n", name, *val);
}

void print_outputs()
{
#define __LOCATED_VAR(type, name, inout, type_sym, a, b) \
    print_out_##inout(#inout #type_sym #a "." #b, POOL_##type##_##inout INDEX_##type(a, b));
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR
}

/*
 *  Linux
 */

#ifdef HW_LINUX
#include <time.h>

static void ts_add(struct timespec *ts, int delay)
{
    ts->tv_nsec += delay;
    if (ts->tv_nsec >= BILLION)
    {
        ts->tv_nsec -= BILLION;
        ts->tv_sec++;
    }
}

void hw_init()
{
    logf("initializing hw\n");
}

void hw_close()
{
    logf("closing hw\n");
}

void update_buffers_in()
{
    DEBUGF("updating input buffers\n");
    print_inputs();
}

void update_buffers_out()
{
    DEBUGF("updating output buffers\n");
    print_outputs();
}

static struct timespec timer;
void timer_init()
{
    clock_gettime(CLOCK_MONOTONIC, &timer);
}

void timer_sleep_until(unsigned long long delay_ns)
{
    ts_add(&timer, delay_ns);
    // NOTE: If the PLC is too slow (real time is > timer), clock_nanosleep
    //       will sleep for no time but `timer` will be
    //       behind wall clock. This error could get corrected in the
    //       subsequent ticks, or can be accumulated.
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer, NULL);

#ifdef TIMER_DEBUG
    static float time_old = 0;
    struct timespec timer2;
    clock_gettime(CLOCK_MONOTONIC, &timer2);
    float time = timer2.tv_sec + timer2.tv_nsec / (float)BILLION;
    DEBUGF("wall time = %.6fs\n", time);
    if (time_old)
    {
        float time_error = (time - time_old) * MILLION - common_ticktime__ / 1000;
        DEBUGF("time_error = %.0fus\n", time_error);
    }
    time_old = time;
#endif
}
#endif

/*
 *  ESP32
 */

#ifdef HW_ESP32
#include <Arduino.h>

// see https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// for PIN choice hints

// digital inputs
const uint8_t DIS_NUM = 4;
const uint8_t DIS_PINS[DIS_NUM] = {16, 17, 5, 18};
// digital outputs
const uint8_t DOS_NUM = 5;
const uint8_t DOS_PINS[DOS_NUM] = {2, 13, 12, 14, 27};
// analog inputs
const uint8_t AIS_NUM = 4;
const uint8_t AIS_PINS[AIS_NUM] = {33, 32, 35, 34}; // 26, 25 does not work?!

void hw_init()
{
    Serial.begin(115200);
    logf("initializing hw\n");
    for (uint8_t i = 0; i < AIS_NUM; i++)
    {
        pinMode(AIS_PINS[i], INPUT);
    }
    for (uint8_t i = 0; i < DIS_NUM; i++)
    {
        pinMode(DIS_PINS[i], INPUT_PULLUP);
    }
    for (uint8_t i = 0; i < DOS_NUM; i++)
    {
        pinMode(DOS_PINS[i], OUTPUT);
    }
}

void hw_close()
{
    logf("closing hw\n");
}

void update_buffers_in()
{
    DEBUGF("updating input buffers\n");
    for (uint8_t i = 0; i < DIS_NUM; i++)
    {
        uint8_t a = i / 8;
        uint8_t b = i % 8;
        IEC_BOOL *val = bool_input[a][b];
        if (val)
        {
            *val = !digitalRead(DIS_PINS[i]);
            DEBUGF("IX%u.%u = %u\n", a, b, *val);
        }
    }
    for (uint8_t i = 0; i < AIS_NUM; i++)
    {
        uint8_t a = i / 8;
        uint8_t b = i % 8;
        IEC_UINT *val = int_input[i];
        if (val)
        {
            *val = analogRead(AIS_PINS[i]);
            DEBUGF("IW%u.%u = %u\n", a, b, *val);
        }
    }
}

void update_buffers_out()
{
    DEBUGF("updating output buffers\n");
    for (uint8_t i = 0; i < DOS_NUM; i++)
    {
        uint8_t a = i / 8;
        uint8_t b = i % 8;
        IEC_BOOL *val = bool_output[a][b];
        if (val)
        {
            DEBUGF("QX%u.%u = %u\n", a, b, *val);
            digitalWrite(DOS_PINS[i], *val);
        }
    }
}

static unsigned long timer;
void timer_init()
{
    timer = micros();
}

void timer_sleep_until(unsigned long long delay_ns)
{
    unsigned long now = micros();

#ifdef TIMER_DEBUG
    float time = now / (float)MILLION;
    DEBUGF("wall time = %.6fs\n", time);
#endif
    // NOTE: We must use subtraction of timers only because micros() will
    // overflow often and subtraction after overflow gives correct results.
    // e.g. 0 - 0xFFFF = 1
    unsigned long elapsed_ns = now - timer;
    long delay_us = (delay_ns - elapsed_ns) / 1000;
    if (delay_us < 0)
    {
        logf("WARNING: timer miss by %ldus", -delay_us);
    }
    else
    {
        DEBUGF("timer margin = %ldus\n", delay_us);
        delayMicroseconds(delay_us);
    }
    timer = now;
}
#endif