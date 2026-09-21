#define _POSIX_C_SOURCE 200809L

#include "os_project.h"

#include <errno.h>
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
 * pause()     -> wait for SIGALRM
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
    printf("Waiting for asynchronous SIGALRM...\n");

    while (!alarm_triggered)
    {
        if (pause() == -1 && errno != EINTR)
        {
            perror("pause");
            return;
        }
    }

    printf("SIGALRM received\n");
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