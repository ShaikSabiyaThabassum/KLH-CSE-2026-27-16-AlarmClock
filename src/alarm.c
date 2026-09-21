#define _POSIX_C_SOURCE 200809L

#include "alarm.h"
#include "os_project.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static Alarm review_alarm_storage;
static Alarm *current_alarm = NULL;

static pthread_t alarm_wait_thread;
static int alarm_wait_thread_started = 0;
static int alarm_wait_cancelled = 0;
static pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *alarm_wait_worker(void *arg);
static void alarm_cancel_wait_thread(void);
static void alarm_trigger(void);

int alarm_init(void)
{
    printf("\nInitializing Alarm Clock...\n");

    /* Review-1 mode: CO4-CO6 integration is disabled.
     * Only CO1-CO3 remain active for this review.
     */
    memset(&review_alarm_storage, 0, sizeof(review_alarm_storage));
    current_alarm = &review_alarm_storage;
    current_alarm->enabled = 0;

    printf("[Review-1] CO4-CO6 integration disabled. CO1-CO3 active.\n");
    printf("Alarm Clock initialized successfully.\n");

    return 0;
}

int alarm_set(int hour,
              int minute,
              int second,
              const char *label)
{
    time_t now;
    time_t target;
    long difference;
    struct tm target_tm;

    alarm_cancel_wait_thread();

    /* Review-1 mode: CO4-CO6 integration is disabled.
     * Only CO1-CO3 remain active.
     */
    co3_cancel_alarm();

    /* CO1 */
    co1_alarm_service();

    if (hour < 0 || hour > 23 ||
        minute < 0 || minute > 59 ||
        second < 0 || second > 59)
    {
        printf("Invalid time.\n");
        return -1;
    }

    /* Review-1 mode: CO4-CO6 integration disabled.
     * Use local alarm memory instead of CO4/CO5/CO6 processes.
     */
    current_alarm = &review_alarm_storage;
    memset(current_alarm, 0, sizeof(*current_alarm));

    current_alarm->hour = hour;
    current_alarm->minute = minute;
    current_alarm->second = second;

    snprintf(current_alarm->label,
             sizeof(current_alarm->label),
             "%s",
             label != NULL ? label : "Alarm");

    current_alarm->enabled = 1;

    now = time(NULL);
    localtime_r(&now, &target_tm);

    target_tm.tm_hour = hour;
    target_tm.tm_min = minute;
    target_tm.tm_sec = second;

    target = mktime(&target_tm);

    if (target <= now)
    {
        target += 24 * 60 * 60;
    }

    difference = (long)(target - now);

    if (difference <= 0)
    {
        difference = 1;
    }

    /* CO3 */
    if (co3_schedule_alarm((unsigned int)difference) != 0)
    {
        printf("Could not schedule SIGALRM.\n");
        return -1;
    }

    /* Review-1 mode: CO6 monitor intentionally disabled.
     * This keeps the project focused on CO1-CO3 only.
     */
    pthread_mutex_lock(&wait_mutex);
    alarm_wait_cancelled = 0;

    if (pthread_create(&alarm_wait_thread,
                       NULL,
                       alarm_wait_worker,
                       NULL) != 0)
    {
        pthread_mutex_unlock(&wait_mutex);

        perror("pthread_create");
        co3_cancel_alarm();
        co6_cancel_monitor();
        return -1;
    }

    alarm_wait_thread_started = 1;
    pthread_mutex_unlock(&wait_mutex);

    printf("\nTime '%s' set for %02d:%02d:%02d.\n",
           current_alarm->label,
           current_alarm->hour,
           current_alarm->minute,
           current_alarm->second);

    return 0;
}

void alarm_view(void)
{
    if (current_alarm == NULL ||
        current_alarm->enabled == 0)
    {
        printf("\nNo time is currently set.\n");
        return;
    }

    printf("\n=====================================\n");
    printf("             STORED TIME\n");
    printf("=====================================\n");

    printf("Time   : %02d:%02d:%02d\n",
           current_alarm->hour,
           current_alarm->minute,
           current_alarm->second);

    printf("Label  : %s\n",
           current_alarm->label);

    printf("Status : %s\n",
           current_alarm->enabled ? "Active" : "Inactive");

    printf("=====================================\n");
}

void alarm_delete(void)
{
    if (current_alarm == NULL ||
        current_alarm->enabled == 0)
    {
        printf("\nNo time is currently set.\n");
        return;
    }

    alarm_cancel_wait_thread();

    /* CO3 */
    co3_cancel_alarm();

    /* Review-1 mode: CO5/CO6 disabled. */
    current_alarm->enabled = 0;

    printf("\nTime cancelled successfully.\n");
}

static void *alarm_wait_worker(void *arg)
{
    int cancelled;

    (void)arg;

    printf("Background alarm waiter is active\n");

    co3_wait_for_alarm();

    pthread_mutex_lock(&wait_mutex);

    cancelled = alarm_wait_cancelled;

    pthread_mutex_unlock(&wait_mutex);

    if (!cancelled)
    {
        alarm_trigger();
    }

    pthread_mutex_lock(&wait_mutex);
    alarm_wait_thread_started = 0;
    pthread_mutex_unlock(&wait_mutex);

    return NULL;
}

static void alarm_cancel_wait_thread(void)
{
    int started;

    pthread_mutex_lock(&wait_mutex);

    started = alarm_wait_thread_started;

    if (started)
    {
        alarm_wait_cancelled = 1;
    }

    pthread_mutex_unlock(&wait_mutex);

    if (!started)
    {
        return;
    }

    co3_cancel_alarm();

    if (pthread_kill(alarm_wait_thread, SIGALRM) != 0)
    {
        /* The join below still cleans up the worker. */
    }

    if (pthread_join(alarm_wait_thread, NULL) != 0)
    {
        perror("pthread_join");
    }

    pthread_mutex_lock(&wait_mutex);
    alarm_wait_thread_started = 0;
    pthread_mutex_unlock(&wait_mutex);
}

static void alarm_trigger(void)
{
    pid_t child_pid;

    if (current_alarm == NULL ||
        current_alarm->enabled == 0)
    {
        return;
    }

    printf("\n============================================\n");
    printf("                 TIME REACHED\n");
    printf("============================================\n");

    printf("Label : %s\n",
           current_alarm->label);

    printf("Time  : %02d:%02d:%02d\n",
           current_alarm->hour,
           current_alarm->minute,
           current_alarm->second);

    /* Review-1 mode: CO6 integration disabled. */

    /* CO2 */
    child_pid = co2_start_alarm_process();

    if (child_pid > 0)
    {
        sleep(3);
        co2_stop_alarm_process(child_pid);
    }

    current_alarm->enabled = 0;

    printf("============================================\n");
    printf("              TIME COMPLETED\n");
    printf("============================================\n");
}

void alarm_wait_and_ring(void)
{
    if (current_alarm == NULL ||
        current_alarm->enabled == 0)
    {
        printf("No active time.\n");
        return;
    }

    printf("Waiting for the alarm...\n");

    co3_wait_for_alarm();

    alarm_trigger();
}

void alarm_stop(void)
{
    alarm_cancel_wait_thread();
    co3_cancel_alarm();

    /* Review-1 mode: CO6 integration disabled. */
    printf("\nTime stopped.\n");
}

void alarm_snooze(int minutes)
{
    time_t snooze_time;
    struct tm snooze_tm;

    if (minutes <= 0)
    {
        printf("Invalid snooze value.\n");
        return;
    }

    snooze_time = time(NULL) + (minutes * 60);

    localtime_r(&snooze_time, &snooze_tm);

    alarm_set(snooze_tm.tm_hour,
              snooze_tm.tm_min,
              snooze_tm.tm_sec,
              "Snoozed");
}

void alarm_shutdown(void)
{
    alarm_cancel_wait_thread();

    co3_cancel_alarm();

    /* Review-1 mode: CO4-CO6 integration disabled. */
    if (current_alarm != NULL)
    {
        current_alarm->enabled = 0;
        current_alarm = NULL;
    }

    pthread_mutex_destroy(&wait_mutex);

    printf("\nAlarm Clock resources released.\n");
}