#ifndef ALARM_H
#define ALARM_H

#include <time.h>

typedef struct
{
    int hour;
    int minute;
    int second;
    char label[64];
    int enabled;
} Alarm;

int alarm_init(void);

int alarm_set(int hour,
              int minute,
              int second,
              const char *label);

void alarm_view(void);

void alarm_delete(void);

void alarm_wait_and_ring(void);

void alarm_stop(void);

void alarm_shutdown(void);

#endif