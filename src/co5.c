#define _POSIX_C_SOURCE 200809L

#include "os_project.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * ============================================================
 * CO5 - FILE SYSTEMS AND FILE I/O
 * ============================================================
 *
 * Actual Alarm Clock use:
 *
 * open()  -> open/create alarm file
 * write() -> store actual alarm
 * read()  -> load actual alarm
 * close() -> close file descriptor
 */

#define ALARM_FILE "data/alarms.txt"

/*
 * ------------------------------------------------------------
 * Save actual alarm
 * ------------------------------------------------------------
 */
int co5_save_alarm(const Alarm *alarm)
{
    int fd;
    char line[256];
    int length;

    if (alarm == NULL)
    {
        return -1;
    }

    fd = open(
        ALARM_FILE,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (fd == -1)
    {
        perror("open");
        return -1;
    }

    length = snprintf(
        line,
        sizeof(line),
        "%02d:%02d:%02d %s %d\n",
        alarm->hour,
        alarm->minute,
        alarm->second,
        alarm->label,
        alarm->enabled
    );

    if (write(fd,
              line,
              (size_t)length) != length)
    {
        perror("write");
        close(fd);
        return -1;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return -1;
    }

    printf("[CO5] Alarm saved to %s using open/write/close\n",
           ALARM_FILE);

    return 0;
}

/*
 * ------------------------------------------------------------
 * Load actual alarm
 * ------------------------------------------------------------
 */
int co5_load_alarm(Alarm *alarm)
{
    int fd;
    char buffer[256];
    ssize_t bytes_read;

    if (alarm == NULL)
    {
        return -1;
    }

    fd = open(ALARM_FILE, O_RDONLY);

    if (fd == -1)
    {
        return -1;
    }

    memset(buffer, 0, sizeof(buffer));

    bytes_read =
        read(
            fd,
            buffer,
            sizeof(buffer) - 1
        );

    if (close(fd) == -1)
    {
        perror("close");
        return -1;
    }

    if (bytes_read <= 0)
    {
        return -1;
    }

    buffer[bytes_read] = '\0';

    if (sscanf(
            buffer,
            "%d:%d:%d %63s %d",
            &alarm->hour,
            &alarm->minute,
            &alarm->second,
            alarm->label,
            &alarm->enabled) != 5)
    {
        return -1;
    }

    printf("[CO5] Alarm loaded from %s using open/read/close\n",
           ALARM_FILE);

    return 0;
}

/*
 * ------------------------------------------------------------
 * Delete stored alarm
 * ------------------------------------------------------------
 */
void co5_delete_alarm(void)
{
    if (unlink(ALARM_FILE) == 0)
    {
        printf("[CO5] Alarm file removed\n");
    }
}

/*
 * ============================================================
 * OPTIONAL CO5 DEMONSTRATION
 * ============================================================
 */
void co5_demo(void)
{
    Alarm demo_alarm;

    memset(&demo_alarm, 0, sizeof(demo_alarm));

    demo_alarm.hour = 10;
    demo_alarm.minute = 30;
    demo_alarm.second = 0;
    demo_alarm.enabled = 1;

    snprintf(
        demo_alarm.label,
        sizeof(demo_alarm.label),
        "DemoAlarm"
    );

    printf("\n========================================\n");
    printf("        CO5 - FILE SYSTEMS AND FILE I/O\n");
    printf("========================================\n");

    if (co5_save_alarm(&demo_alarm) == 0)
    {
        printf("CO5 file-I/O demonstration completed successfully.\n");
    }
}