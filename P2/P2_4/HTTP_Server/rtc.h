#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include "rl_net.h"
#include <stdio.h>
#include <time.h>


// Prototipos de las funciones 
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

#define LED1_ON  0x01
#define LED2_ON  0x02
#define LED3_ON  0x04
#define BLUE_ON   0x08
#define GREEN_ON  0x10
#define RED_ON 0x20

//uint8_t aShowTime[50] = {0};
//uint8_t aShowDate[50] = {0};


void RTC_Init(void);
void RTC_CalendarConfig(void);
void RTC_SetAlarm(void);

void RTC_CalendarShow(void);
void Init_Timers(void);
void init_Leds (void);

void Rtc_Alarm_IRQHandler(void);
void RTC_Alarm_Config(void);

void Toggle_ldV(void);
void Toggle_ldR(void);

void TextoWeb(void);

void get_time(void);
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);
void reset_tiempo(void);
#endif