#define _POSIX_C_SOURCE 200809L

#include "alarm.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Display the current time and the Alarm Clock menu.
 */
static void show_menu(void)
{
    time_t now;
    struct tm current_time;

    now = time(NULL);

    if (localtime_r(&now, &current_time) == NULL)
    {
        printf("\nCurrent Time: unavailable\n");
    }
    else
    {
        printf("\nCurrent Time: %02d:%02d:%02d\n",
               current_time.tm_hour,
               current_time.tm_min,
               current_time.tm_sec);
    }

    printf("\n============================================================\n");
    printf("                 LINUX TERMINAL ALARM CLOCK\n");
    printf("============================================================\n");
    printf("  1. Set Alarm\n");
    printf("  2. View Alarm\n");
    printf("  3. Cancel Alarm\n");
    printf("  4. Exit\n");
    printf("============================================================\n");
}

int main(void)
{
    int choice;

    /*
     * Initialize the complete Alarm Clock system.
     * CO1-CO6 are connected through alarm.c.
     */
    if (alarm_init() != 0)
    {
        fprintf(stderr,
                "Could not initialize Alarm Clock.\n");

        return EXIT_FAILURE;
    }

    while (1)
    {
        show_menu();

        printf("Enter choice: ");

        /*
         * Read menu choice.
         */
        if (scanf("%d", &choice) != 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }

            printf("Please enter a number from 1 to 4.\n");
            continue;
        }

        switch (choice)
        {
            /*
             * ====================================================
             * SET ALARM
             * ====================================================
             */
            case 1:
            {
                int hour;
                int minute;
                int second;
                char label[64];

                printf("\nEnter alarm hour (0-23): ");

                if (scanf("%d", &hour) != 1)
                {
                    printf("Invalid hour.\n");
                    break;
                }

                printf("Enter alarm minute (0-59): ");

                if (scanf("%d", &minute) != 1)
                {
                    printf("Invalid minute.\n");
                    break;
                }

                printf("Enter alarm second (0-59): ");

                if (scanf("%d", &second) != 1)
                {
                    printf("Invalid second.\n");
                    break;
                }

                printf("Enter label (no spaces, e.g. WakeUp): ");

                if (scanf("%63s", label) != 1)
                {
                    printf("Invalid label.\n");
                    break;
                }

                /*
                 * alarm_set() is the main project operation.
                 * alarm.c connects the required CO1-CO6 modules.
                 */
                if (alarm_set(hour, minute, second, label) == 0)
                {
                    printf("\nAlarm saved successfully.\n");

                    /*
                     * Wait until the alarm time and ring.
                     */
                    alarm_wait_and_ring();
                }

                break;
            }

            /*
             * ====================================================
             * VIEW ALARM
             * ====================================================
             */
            case 2:
                alarm_view();
                break;

            /*
             * ====================================================
             * CANCEL ALARM
             * ====================================================
             */
            case 3:
                alarm_delete();
                break;

            /*
             * ====================================================
             * EXIT
             * ====================================================
             */
            case 4:
                alarm_shutdown();

                printf("\nGoodbye.\n");

                return EXIT_SUCCESS;

            /*
             * ====================================================
             * INVALID CHOICE
             * ====================================================
             */
            default:
                printf("Invalid choice. Please select 1-4.\n");
        }
    }

    return EXIT_SUCCESS;
}