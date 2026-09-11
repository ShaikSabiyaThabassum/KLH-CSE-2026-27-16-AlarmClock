#include <stdio.h>

/* Function provided by co3.c */
void start_alarm(int seconds);

int main(void)
{
    int seconds;

    printf("========================================\n");
    printf("          CO3 ALARM CLOCK TEST\n");
    printf("========================================\n");

    printf("Enter alarm duration in seconds: ");

    if (scanf("%d", &seconds) != 1)
    {
        printf("Invalid input.\n");
        return 1;
    }

    start_alarm(seconds);

    return 0;
}