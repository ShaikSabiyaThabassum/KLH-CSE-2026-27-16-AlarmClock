#define _POSIX_C_SOURCE 200809L

#include "os_project.h"

#include <pthread.h>
#include <stdio.h>

/*
 * ============================================================
 * CO6 - THREADS, MUTEX AND SYNCHRONIZATION
 * ============================================================
 *
 * Actual Alarm Clock use:
 *
 * pthread_create()      -> create monitor thread
 * pthread_mutex_lock()  -> protect shared alarm
 * pthread_mutex_unlock()-> release shared alarm
 * pthread_cond_wait()   -> wait for alarm event
 * pthread_cond_signal() -> notify monitor thread
 * pthread_join()        -> wait for thread completion
 */

static pthread_mutex_t alarm_mutex =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t alarm_cond =
    PTHREAD_COND_INITIALIZER;

static pthread_t monitor_thread;

static int monitor_started = 0;
static int alarm_event = 0;
static int cancel_event = 0;

static Alarm *shared_alarm = NULL;

/*
 * ------------------------------------------------------------
 * Monitor thread
 * ------------------------------------------------------------
 */
static void *monitor_alarm(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&alarm_mutex);

    printf("[CO6] Monitor thread started\n");

    while (!alarm_event && !cancel_event)
    {
        pthread_cond_wait(
            &alarm_cond,
            &alarm_mutex
        );
    }

    if (cancel_event)
    {
        printf("[CO6] Monitor thread received cancellation event\n");
    }

    if (alarm_event && shared_alarm != NULL)
    {
        printf("[CO6] Mutex-protected alarm state accessed\n");

        printf("[CO6] Alarm: %02d:%02d:%02d (%s)\n",
               shared_alarm->hour,
               shared_alarm->minute,
               shared_alarm->second,
               shared_alarm->label);
    }

    pthread_mutex_unlock(&alarm_mutex);

    printf("[CO6] Monitor thread finished\n");

    return NULL;
}

/*
 * ------------------------------------------------------------
 * Lock shared alarm
 * ------------------------------------------------------------
 */
void co6_lock(void)
{
    pthread_mutex_lock(&alarm_mutex);
}

/*
 * ------------------------------------------------------------
 * Unlock shared alarm
 * ------------------------------------------------------------
 */
void co6_unlock(void)
{
    pthread_mutex_unlock(&alarm_mutex);
}

/*
 * ------------------------------------------------------------
 * Start actual Alarm Clock monitor thread
 * ------------------------------------------------------------
 */
int co6_start_monitor(Alarm *alarm)
{
    if (monitor_started)
    {
        return 0;
    }

    pthread_mutex_lock(&alarm_mutex);

    shared_alarm = alarm;
    alarm_event = 0;
    cancel_event = 0;

    if (pthread_create(
            &monitor_thread,
            NULL,
            monitor_alarm,
            NULL) != 0)
    {
        pthread_mutex_unlock(&alarm_mutex);

        perror("pthread_create");
        return -1;
    }

    monitor_started = 1;

    pthread_mutex_unlock(&alarm_mutex);

    printf("[CO6] POSIX monitor thread created\n");

    return 0;
}

/*
 * ------------------------------------------------------------
 * Notify monitor thread
 * ------------------------------------------------------------
 */
void co6_notify_alarm(void)
{
    pthread_mutex_lock(&alarm_mutex);

    alarm_event = 1;

    pthread_cond_signal(&alarm_cond);

    pthread_mutex_unlock(&alarm_mutex);

    printf("[CO6] Alarm event signalled to monitor thread\n");
}

/*
 * ------------------------------------------------------------
 * Join actual monitor thread
 * ------------------------------------------------------------
 */
void co6_join_monitor(void)
{
    if (!monitor_started)
    {
        return;
    }

    if (pthread_join(
            monitor_thread,
            NULL) != 0)
    {
        perror("pthread_join");
        return;
    }

    monitor_started = 0;
    shared_alarm = NULL;

    printf("[CO6] pthread_join() completed\n");
}

/*
 * ------------------------------------------------------------
 * Cancel actual monitor thread
 * ------------------------------------------------------------
 */
void co6_cancel_monitor(void)
{
    if (!monitor_started)
    {
        return;
    }

    pthread_mutex_lock(&alarm_mutex);

    cancel_event = 1;

    pthread_cond_signal(&alarm_cond);

    pthread_mutex_unlock(&alarm_mutex);

    if (pthread_join(
            monitor_thread,
            NULL) != 0)
    {
        perror("pthread_join");
        return;
    }

    monitor_started = 0;
    shared_alarm = NULL;

    printf("[CO6] Monitor thread cancelled safely\n");
}

/*
 * ------------------------------------------------------------
 * Shutdown synchronization resources
 * ------------------------------------------------------------
 */
void co6_shutdown(void)
{
    if (monitor_started)
    {
        co6_cancel_monitor();
    }

    pthread_mutex_destroy(&alarm_mutex);
    pthread_cond_destroy(&alarm_cond);
}

/*
 * ============================================================
 * OPTIONAL CO6 DEMONSTRATION
 * ============================================================
 */
void co6_demo(void)
{
    Alarm demo_alarm;

    printf("\n========================================\n");
    printf("       CO6 - THREADS AND SYNCHRONIZATION\n");
    printf("========================================\n");

    demo_alarm.hour = 10;
    demo_alarm.minute = 30;
    demo_alarm.second = 0;
    demo_alarm.enabled = 1;

    snprintf(
        demo_alarm.label,
        sizeof(demo_alarm.label),
        "DemoAlarm"
    );

    if (co6_start_monitor(&demo_alarm) != 0)
    {
        return;
    }

    printf("[CO6] Main thread continues\n");

    co6_notify_alarm();

    co6_join_monitor();

    printf("CO6 thread/mutex demonstration completed successfully.\n");
}