#include <time.h>

#ifdef HW_LINUX
#include <stdlib.h> // exit(...)
#endif

#include "iec_types.h"

#include "hal.h"

//MatIEC Compiler
void config_run__(unsigned long tick);
void config_init__(void);

//vars.cpp
void glue_vars();

//main.cpp
void sleep_until(struct timespec *ts, int delay);

//Common task timer
extern unsigned long long common_ticktime__;

static int tick = 0;
static uint8_t run_openplc = 1;

extern IEC_TIME __CURRENT_TIME;

/*
 * INITIALIZATION
 */
void setup()
{
    logf("PLC initialization\n");

    logf("\tPLC ...\n");
    config_init__();
    glue_vars();
    logf("\t\t\tOK\n");

    // HARDWARE INITIALIZATION
    logf("\tHW ...\n");
    hw_init();
    update_buffers_in();
    update_buffers_out();
    logf("\t\t\tOK\n");

    // gets the starting point for the clock
    logf("\ttimer ...\n");
    timer_init();
    logf("\t\t\tOK\n");

    logf("initialization DONE\n");
}

/*
 * MAIN LOOP
 */
void loop()
{
    while (run_openplc)
    {
#ifdef DEBUG
        float time = __CURRENT_TIME.tv_sec + __CURRENT_TIME.tv_nsec / (float)1000000000;
        DEBUGF("\ntick = %d, time = %.3fs\n", tick, time);
#endif
        update_buffers_in();
        config_run__(tick++); // execute plc program logic
        update_buffers_out();
        update_time();
        timer_sleep_until(common_ticktime__);
    }

    // SHUT DOWN
    update_buffers_out();
    hw_close();
    logf("PLC shutting down\n");
#ifdef HW_LINUX
    exit(0);
#endif
#ifdef HW_ESP32
    for (;;)
    {
        delay(10000);
    }
#endif
}

#ifdef HW_LINUX
int main(int argc, char **argv)
{
    setup();
    loop();
}
#endif