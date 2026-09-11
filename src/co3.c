
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/*
 * CO3: Signals and Asynchronous Notifications
 *
 * Linux/POSIX concepts implemented:
 * 1. alarm()
 * 2. SIGALRM
 * 3. sigaction()
 * 4. Signal handler
 * 5. pause()
 */

static volatile sig_atomic_t alarm_triggered = 0;

/*
 * Signal handler for SIGALRM
 */
static void alarm_handler(int signal_number)
{
    (void)signal_number;

    const char message[] =
        "\n========================================\n"
        "              !!! ALARM !!!\n"
        "        Your alarm is ringing!\n"
        "========================================\n";

    /*
     * Set the flag to indicate that the alarm
     * has been triggered.
     */
    alarm_triggered = 1;

    /*
     * Display the alarm notification.
     * write() is used inside the signal handler.
     */
    write(STDOUT_FILENO, message, sizeof(message) - 1);
}

/*
 * Start the alarm timer.
 */
void start_alarm(int seconds)
{
    struct sigaction sa;

    /*
     * Do not allow zero or negative values.
     */
    if (seconds <= 0)
    {
        printf("Error: alarm time must be greater than 0 seconds.\n");
        return;
    }

    /*
     * Prepare the signal action structure.
     */
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /*
     * Register the handler for SIGALRM.
     */
    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        perror("sigaction");
        return;
    }

    /*
     * Reset the alarm status.
     */
    alarm_triggered = 0;

    printf("\nAlarm set for %d seconds.\n", seconds);
    printf("Waiting for SIGALRM...\n");

    /*
     * Start the timer.
     */
    alarm((unsigned int)seconds);

    /*
     * Wait until SIGALRM is received.
     */
    while (!alarm_triggered)
    {
        if (pause() == -1 && errno != EINTR)
        {
            perror("pause");
            return;
        }
    }

    printf("CO3 alarm completed successfully.\n");
}