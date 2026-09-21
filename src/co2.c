#define _POSIX_C_SOURCE 200809L

#include "os_project.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * ============================================================
 * CO2 - PROCESSES AND PROCESS CONTROL
 * ============================================================
 *
 * Alarm Clock application use:
 *
 * fork()    -> create child process for alarm sound
 * kill()    -> parent controls/stops child
 * waitpid() -> parent waits for child termination
 */

/*
 * ------------------------------------------------------------
 * Start actual Alarm Clock sound process
 * ------------------------------------------------------------
 */
pid_t co2_start_alarm_process(void)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    /*
     * Child process.
     */
    if (pid == 0)
    {
        printf("Child process started: PID %ld\n",
               (long)getpid());

        /*
         * Produce terminal alarm notifications.
         * This works in Ubuntu/WSL without requiring
         * additional audio packages.
         */
        execl(
            "/bin/sh",
            "sh",
            "-c",
            "for i in 1 2 3 4 5 6; do printf '\\a'; sleep 0.5; done",
            (char *)NULL
        );

        /*
         * Only reached when exec fails.
         */
        _exit(127);
    }

    /*
     * Parent process.
     */
    printf("Alarm child process created: PID %ld\n",
           (long)pid);

    return pid;
}

/*
 * ------------------------------------------------------------
 * Stop and wait for actual Alarm Clock process
 * ------------------------------------------------------------
 */
void co2_stop_alarm_process(pid_t pid)
{
    int status;

    if (pid <= 0)
    {
        return;
    }

    /*
     * Parent sends SIGTERM to the child.
     */
    printf("Parent sends SIGTERM using kill()\n");

    if (kill(pid, SIGTERM) == -1)
    {
        perror("kill");
    }

    /*
     * Parent waits for child.
     */
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        return;
    }

    if (WIFSIGNALED(status))
    {
        printf("Child terminated by signal %d\n",
               WTERMSIG(status));
    }
    else if (WIFEXITED(status))
    {
        printf("Child exited with status %d\n",
               WEXITSTATUS(status));
    }

    printf("Process control completed\n");
}

/*
 * ============================================================
 * OPTIONAL CO2 DEMONSTRATION
 * ============================================================
 *
 * This is retained so the file clearly shows the CO2 concept
 * during source-code review/viva.
 */
void co2_demo(void)
{
    pid_t child_pid;
    int status;

    printf("\n========================================\n");
    printf("       CO2 - PROCESSES AND PROCESS CONTROL\n");
    printf("========================================\n");

    child_pid = fork();

    if (child_pid < 0)
    {
        perror("fork");
        return;
    }

    if (child_pid == 0)
    {
        printf("Child process started.\n");
        printf("Child PID : %ld\n", (long)getpid());
        printf("Child PPID: %ld\n", (long)getppid());

        pause();

        _exit(EXIT_SUCCESS);
    }

    printf("Parent PID       : %ld\n", (long)getpid());
    printf("Created child PID: %ld\n", (long)child_pid);

    sleep(1);

    printf("Parent sending SIGTERM to child...\n");

    if (kill(child_pid, SIGTERM) == -1)
    {
        perror("kill");
        return;
    }

    if (waitpid(child_pid, &status, 0) == -1)
    {
        perror("waitpid");
        return;
    }

    printf("CO2 demonstration completed successfully.\n");
}