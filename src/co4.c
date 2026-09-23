#define _POSIX_C_SOURCE 200809L

#include "os_project.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * ============================================================
 * CO4 - MEMORY MANAGEMENT
 * ============================================================
 *
 * Actual Alarm Clock use:
 *
 * calloc() / malloc() -> allocate alarm structure
 * free()              -> release alarm structure
 */

/*
 * ------------------------------------------------------------
 * Create actual Alarm Clock memory
 * ------------------------------------------------------------
 */
Alarm *co4_create_alarm(void)
{
    Alarm *alarm_data;

    alarm_data =
        (Alarm *)calloc(1, sizeof(Alarm));

    if (alarm_data == NULL)
    {
        perror("calloc");
        return NULL;
    }

    printf("Alarm memory allocated dynamically\n");

    return alarm_data;
}

/*
 * ------------------------------------------------------------
 * Release actual Alarm Clock memory
 * ------------------------------------------------------------
 */
void co4_destroy_alarm(Alarm *alarm_data)
{
    if (alarm_data == NULL)
    {
        return;
    }

    free(alarm_data);

    printf("Alarm memory released using free()\n");
}

/*
 * ============================================================
 * OPTIONAL CO4 DEMONSTRATION
 * ============================================================
 */
void co4_demo(void)
{
    Alarm *alarm_data;

    printf("\n========================================\n");
    printf("          CO4 - MEMORY MANAGEMENT\n");
    printf("========================================\n");

    alarm_data =
        (Alarm *)malloc(sizeof(Alarm));

    if (alarm_data == NULL)
    {
        perror("malloc");
        return;
    }

    alarm_data->hour = 10;
    alarm_data->minute = 30;
    alarm_data->second = 0;
    alarm_data->enabled = 1;

    snprintf(alarm_data->label,
             sizeof(alarm_data->label),
             "DemoAlarm");

    printf("malloc() allocated alarm memory\n");

    printf("Alarm Time: %02d:%02d:%02d\n",
           alarm_data->hour,
           alarm_data->minute,
           alarm_data->second);

    printf("Alarm Status: %s\n",
           alarm_data->enabled
               ? "Active"
               : "Inactive");

    free(alarm_data);

    alarm_data = NULL;

    printf("free() released alarm memory\n");
    printf("CO4 demonstration completed successfully.\n");
}