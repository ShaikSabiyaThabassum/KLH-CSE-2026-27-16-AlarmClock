#define _GNU_SOURCE

#include "os_project.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

/*
 * ============================================================
 * CO1 - OS AS A SERVICE LAYER
 * ============================================================
 *
 * Main concepts demonstrated:
 *
 * 1. getpid()  -> current process ID
 * 2. getppid() -> parent process ID
 * 3. write()   -> Linux system-call based output
 * 4. uname()   -> operating-system/kernel information
 * 5. open()    -> file system service
 * 6. close()   -> close file descriptor
 */

/*
 * ============================================================
 * CO1 DEMONSTRATION
 * ============================================================
 *
 * This function can be used when Ma'am asks to demonstrate CO1.
 */
void co1_demo(void)
{
    printf("\n========================================\n");
    printf("       CO1 - OS AS A SERVICE LAYER\n");
    printf("========================================\n");

    /*
     * --------------------------------------------------------
     * 1. Process information
     * --------------------------------------------------------
     */
    printf("\n[1] Linux process information\n");

    printf("Current Process ID : %ld\n", (long)getpid());
    printf("Parent Process ID  : %ld\n", (long)getppid());

    /*
     * --------------------------------------------------------
     * 2. Operating-system information
     * --------------------------------------------------------
     */
    printf("\n[2] Linux operating-system information\n");

    struct utsname system_info;

    if (uname(&system_info) == 0)
    {
        printf("Operating System : %s\n", system_info.sysname);
        printf("Kernel Release   : %s\n", system_info.release);
        printf("Machine          : %s\n", system_info.machine);
    }
    else
    {
        perror("uname");
    }

    /*
     * --------------------------------------------------------
     * 3. write() system call
     * --------------------------------------------------------
     */
    printf("\n[3] write() system call\n");

    const char message[] =
        "write() is being used as a Linux system call.\n";

    ssize_t bytes_written =
        write(STDOUT_FILENO, message, sizeof(message) - 1);

    if (bytes_written < 0)
    {
        perror("write");
    }
    else
    {
        printf("write() completed successfully.\n");
    }

    /*
     * --------------------------------------------------------
     * 4. open() and close()
     * --------------------------------------------------------
     *
     * These demonstrate another interaction with a Linux
     * kernel service through a file descriptor.
     */
    printf("\n[4] open() and close() system calls\n");

    const char *file_path = "data/co1_syscall_demo.txt";

    int fd = open(
        file_path,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (fd == -1)
    {
        perror("open");
        return;
    }

    const char file_message[] =
        "Linux open/write/close system-call demonstration.\n";

    ssize_t file_bytes =
        write(fd, file_message, sizeof(file_message) - 1);

    if (file_bytes < 0)
    {
        perror("write");
    }
    else
    {
        printf("File written successfully.\n");
    }

    if (close(fd) == -1)
    {
        perror("close");
    }
    else
    {
        printf("File descriptor closed successfully.\n");
    }

    printf("\nCO1 completed successfully.\n");
}

/*
 * ============================================================
 * CO1 - ACTUAL ALARM CLOCK CONNECTION
 * ============================================================
 *
 * This function is called by main.c when the user selects
 * "Set Alarm".
 *
 * Therefore CO1 is not a separate program. It becomes part
 * of the actual Alarm Clock execution.
 */
void co1_alarm_service(void)
{
    struct utsname system_info;

    printf("\nOS service layer active\n");

    /*
     * Process information supplied by the operating system.
     */
    printf("Current Process ID : %ld\n",
           (long)getpid());

    printf("Parent Process ID  : %ld\n",
           (long)getppid());

    /*
     * Operating-system information.
     */
    if (uname(&system_info) == 0)
    {
        printf("Operating System   : %s\n",
               system_info.sysname);

        printf("Kernel Release     : %s\n",
               system_info.release);
    }
    else
    {
        perror("uname");
    }

    /*
     * Use write() as a Linux system call.
     */
    {
        const char message[] =
            "write() system call used by Alarm Clock.\n";

        if (write(STDOUT_FILENO,
                  message,
                  sizeof(message) - 1) < 0)
        {
            perror("write");
        }
    }

    printf("System-service step completed.\n");
}