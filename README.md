Project Title: Alarm Clock

Section: 6
Team Number: 16

Team Members:
2520030537 - Monisha Raghini Chelle
2520030502 - Shaik Sabiya Tabassum
2520030453 - Kolipaka Siri Chandana

Description:
A Linux-based terminal Alarm Clock developed in C using POSIX system calls,
process control, and signal-based alarm handling.

Review-1 Mode:
This project is currently configured for Review-1 demonstration, where the
real application flow focuses on CO1, CO2, and CO3.

- CO1: OS service layer and Linux system calls
- CO2: process creation and process control
- CO3: SIGALRM and signal handling

CO4, CO5, and CO6 files are kept in the project but their integration is
intentionally disabled for this review to keep the demonstration focused on
CO1 to CO3. The project still contains one main entry point in main.c.

Menu:
1. Set Time
2. View Time
3. Cancel Time
4. Exit

Build command (WSL/Linux):
gcc -Wall -Wextra -std=c11 -pthread src/main.c src/alarm.c src/co1.c src/co2.c src/co3.c src/co4.c src/co5.c src/co6.c -o alarm_clock

Run command:
./alarm_clock