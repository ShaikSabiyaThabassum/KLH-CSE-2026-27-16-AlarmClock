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

CO Integration:
The terminal application includes all six CO modules through alarm.c. They
run at different points in the alarm workflow; they are not all executed
simultaneously.

- CO1: reports OS/process information when an alarm is set
- CO2: creates and controls the child process that rings when the alarm fires
- CO3: schedules, waits for, and cancels the SIGALRM timer
- CO4: allocates and releases alarm memory
- CO5: loads, saves, and deletes alarm data
- CO6: starts and signals the POSIX monitor thread

The separate co1_demo() through co6_demo() functions are not menu options;
the main application uses each module's alarm-service functions instead.
There is one application entry point in main.c.

Menu:
1. Set Alarm
2. View Alarm
3. Cancel Alarm
4. Exit

Build status: all application and CO source files pass a warning-enabled GCC
syntax check. This does not verify linking or every runtime path. A saved
alarm is loaded at startup but its timer is not restarted; replacing an active
alarm may leave the CO6 monitor using the previous alarm state. These
behaviors need fixes before claiming that every alarm workflow runs correctly.

Build command (WSL/Linux):

gcc -Wall -Wextra -std=c11 -pthread src/main.c src/alarm.c src/co1.c src/co2.c src/co3.c src/co4.c src/co5.c src/co6.c -o alarm_clock
Run command:
./alarm_clock