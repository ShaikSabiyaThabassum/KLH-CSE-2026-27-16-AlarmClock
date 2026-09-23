#define _POSIX_C_SOURCE 200809L

#include "os_project.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * ============================================================
 * CO3 - IPC, SIGNALS AND ASYNCHRONOUS NOTIFICATION
 * ============================================================
 *
 * Actual Alarm Clock use:
 *
 * sigaction() -> install signal handler
 * alarm()     -> schedule alarm
 * SIGALRM     -> asynchronous alarm notification
 * sigwait()   -> wait for SIGALRM in the alarm worker
 */

static volatile sig_atomic_t alarm_triggered = 0;

/*
 * ------------------------------------------------------------
 * SIGALRM handler
 * ------------------------------------------------------------
 */
static void alarm_handler(int signo)
{
    (void)signo;

    alarm_triggered = 1;
}

/*
 * ------------------------------------------------------------
 * Schedule actual Alarm Clock timer
 * ------------------------------------------------------------
 */
int co3_schedule_alarm(unsigned int seconds)
{
    struct sigaction sa;
    sigset_t alarm_set;
    int result;

    if (seconds == 0)
    {
        seconds = 1;
    }

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /*
     * Register handler for SIGALRM.
     */
    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        perror("sigaction");
        return -1;
    }

    sigemptyset(&alarm_set);
    sigaddset(&alarm_set, SIGALRM);

    /* Keep SIGALRM in the worker's wait set instead of waking the menu. */
    result = pthread_sigmask(SIG_BLOCK, &alarm_set, NULL);

    if (result != 0)
    {
        fprintf(stderr,
                "pthread_sigmask: %s\n",
                strerror(result));
        return -1;
    }

    alarm_triggered = 0;

    /*
     * Start Linux/POSIX alarm timer.
     */
    alarm(seconds);

    printf("SIGALRM timer scheduled for %u seconds\n",
           seconds);

    return 0;
}

/*
 * ------------------------------------------------------------
 * Wait for the actual alarm
 * ------------------------------------------------------------
 */
void co3_wait_for_alarm(void)
{
    sigset_t alarm_set;
    int signal_number;
    int result;

    printf("Waiting for asynchronous SIGALRM...\n");

    sigemptyset(&alarm_set);
    sigaddset(&alarm_set, SIGALRM);

    result = sigwait(&alarm_set, &signal_number);

    if (result != 0)
    {
        fprintf(stderr,
                "sigwait: %s\n",
                strerror(result));
        return;
    }

    alarm_triggered = 1;

    printf("SIGALRM received.\n");
}

/*
 * ------------------------------------------------------------
 * Cancel actual alarm timer
 * ------------------------------------------------------------
 */
void co3_cancel_alarm(void)
{
    alarm(0);
    alarm_triggered = 0;
    printf("SIGALRM timer cancelled\n");
}

/*
 * ============================================================
 * OPTIONAL CO3 DEMONSTRATION
 * ============================================================
 *
 * Retained for viva/source-code explanation.
 */
static volatile sig_atomic_t demo_signal_received = 0;

static void demo_handler(int signo)
{
    (void)signo;
    demo_signal_received = 1;
}

void co3_demo(void)
{
    struct sigaction sa;

    printf("\n========================================\n");
    printf("      CO3 - IPC AND SIGNALS\n");
    printf("========================================\n");

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = demo_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        return;
    }

    demo_signal_received = 0;

    if (kill(getpid(), SIGUSR1) == -1)
    {
        perror("kill");
        return;
    }

    if (demo_signal_received)
    {
        printf("SIGUSR1 received asynchronously.\n");
    }

    printf("Alarm Clock uses SIGALRM with alarm() and sigaction().\n");
    printf("CO3 demonstration completed successfully.\n");
}