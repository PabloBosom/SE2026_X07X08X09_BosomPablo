#ifndef TH_LED_H
#define TH_LED_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file

#include "main.h"

void Thread_TST (void *argument);                   // thread function
int Init_MsgQueue(void);

int Init_ThLeds (void);
void ThLeds (void *argument);                   // thread function
void initLeds(void);

extern  osThreadId_t tid_ThLeds;

#endif
