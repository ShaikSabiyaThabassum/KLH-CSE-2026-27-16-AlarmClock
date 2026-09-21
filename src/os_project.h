#ifndef OS_PROJECT_H
#define OS_PROJECT_H

#include "alarm.h"
#include <pthread.h>
#include <sys/types.h>

/* CO1 */
void co1_alarm_service(void);
void co1_demo(void);

/* CO2 */
pid_t co2_start_alarm_process(void);
void co2_stop_alarm_process(pid_t pid);
void co2_demo(void);

/* CO3 */
int co3_schedule_alarm(unsigned int seconds);
void co3_wait_for_alarm(void);
void co3_cancel_alarm(void);
void co3_demo(void);

/* CO4 */
Alarm *co4_create_alarm(void);
void co4_destroy_alarm(Alarm *alarm);
void co4_demo(void);

/* CO5 */
int co5_save_alarm(const Alarm *alarm);
int co5_load_alarm(Alarm *alarm);
void co5_delete_alarm(void);
void co5_demo(void);

/* CO6 */
void co6_lock(void);
void co6_unlock(void);
int co6_start_monitor(Alarm *alarm);
void co6_notify_alarm(void);
void co6_join_monitor(void);
void co6_cancel_monitor(void);
void co6_shutdown(void);
void co6_demo(void);

#endif