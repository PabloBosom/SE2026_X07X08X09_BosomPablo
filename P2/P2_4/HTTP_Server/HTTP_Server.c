/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"
#include "LCD.h"
#include "adc.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "rtc.h"
#include "pulsador.h"
#include "TH_LED.h"
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD


// Declaración externa del manejador RTC
extern RTC_HandleTypeDef RtcHandle;
extern void RTC_SetAlarm(void);


// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

//extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];



extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
osThreadId_t Tid_RTC;
osThreadId_t TID_SNTP;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
                            
 osMessageQueueId_t  mid_MsgQueue_LCD;


/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
                           
                           
 ADC_HandleTypeDef adchandle;
 uint16_t ADC_value = 0;
__NO_RETURN void app_main (void *arg);

                           
 //creamos el hilo E4
   static void SNTP_Tread(void *arg);
   osThreadId_t  TID_SNTP;
   
/* Read analog inputs 
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }
  return ((uint16_t)val);
}
*/

uint16_t AD_in (uint32_t ch) {
 ADC_HandleTypeDef adchandle;
 uint16_t ADC_value = 0;
 ADC1_pins_F429ZI_config();
 ADC_Init_Single_Conversion(&adchandle , ADC1);
  
  if(ch == 0)
  {
    ADC_value = ADC_getVoltage(&adchandle , 10 );
  }
   return ((uint16_t)ADC_value);
}
/* Read digital inputs */
uint8_t get_button (void) {
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
/*static __NO_RETURN void Display (void *arg) {
  static uint8_t ip_addr[NET_ADDR_IP6_LEN];
  static char    ip_ascii[40];
  static char    buf[24];
  uint32_t x = 0;

  (void)arg;

  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (x*16U, 1U*24U, "       MDK-MW       ");
  GLCD_DrawString         (x*16U, 2U*24U, "HTTP Server example ");

  GLCD_DrawString (x*16U, 4U*24U, "IP4:Waiting for DHCP");
*/
  /* Print Link-local IPv6 address 
  netIF_GetOption (NET_IF_CLASS_ETH,
                   netIF_OptionIP6_LinkLocalAddress, ip_addr, sizeof(ip_addr));

  netIP_ntoa(NET_ADDR_IP6, ip_addr, ip_ascii, sizeof(ip_ascii));

  sprintf (buf, "IP6:%.16s", ip_ascii);
  GLCD_DrawString ( x    *16U, 5U*24U, buf);
  sprintf (buf, "%s", ip_ascii+16);
  GLCD_DrawString ((x+10U)*16U, 6U*24U, buf);

  while(1) {
    *//* Wait for signal from DHCP 
    osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);

    *//* Retrieve and print IPv4 address 
    netIF_GetOption (NET_IF_CLASS_ETH,
                     netIF_OptionIP4_Address, ip_addr, sizeof(ip_addr));

    netIP_ntoa (NET_ADDR_IP4, ip_addr, ip_ascii, sizeof(ip_ascii));

    sprintf (buf, "IP4:%-16s",ip_ascii);
    GLCD_DrawString (x*16U, 4U*24U, buf);

    *//* Display user text lines 
    sprintf (buf, "%-20s", lcd_text[0]);
    GLCD_DrawString (x*16U, 7U*24U, buf);
    sprintf (buf, "%-20s", lcd_text[1]);
    GLCD_DrawString (x*16U, 8U*24U, buf);
  }
}
*/
/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}
//static __NO_RETURN void Display (void *arg)
//{
//  (void)arg;
//  while(1)
//  {
//     RTC_CalendarShow(aShowTime,aShowDate);

//    snprintf(lcd_text[0],20,"%s",aShowTime);
//      snprintf(lcd_text[1],20,"%s",aShowDate);
//    osThreadFlagsWait(0x01,osFlagsWaitAny, osWaitForever); //Tener cuidado por si es un 0x02 
//    
//      osDelay(50);
//  snprintf(msg_lcd.Buf,sizeof(msg_lcd.Buf),"%s",lcd_text[0]); 
//  msg_lcd.Idx = 0;
//  osMessageQueuePut(mid_MsgQueue_LCD,&msg_lcd,0U,0U);
//  
//  osDelay(50);
//  snprintf(msg_lcd.Buf,sizeof(msg_lcd.Buf),"%s",lcd_text[1]);
//  msg_lcd.Idx = 1;
//  osMessageQueuePut(mid_MsgQueue_LCD,&msg_lcd,0U,0U);
//  }
//  
//}

__NO_RETURN void RTC_Thread (void *arg) {
  (void)arg;
  
  RTC_Init();
  RTC_CalendarConfig();
  
  
  while(1)
  {
   if(osThreadFlagsWait(0x0C,osFlagsWaitAll,0)== 0x0C)
   {
     RTC_CalendarShow();
   }
   if(osThreadFlagsWait(0x12,osFlagsWaitAll,0)== 0x12) 
   {
     RTC_Alarm_Config();
      Toggle_ldV();
     osThreadFlagsClear(0x12);
   }
   }
}

__NO_RETURN void SNTP_Thread (void *arg) {


  (void)arg;
 Init_pulsador();
  osDelay(5000);
  
  while(1)
  {
   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET){
      get_time();
   }
   }
}
/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

//  LED_Initialize();
//  Buttons_Initialize();
  //ADC_Initialize();
 mid_MsgQueue_LCD =  osMessageQueueNew(4, sizeof(MSGQUEUE_OBJ_t_LCD), NULL);
   
//  ADC1_pins_F429ZI_config();
// ADC_Init_Single_Conversion(&adchandle , ADC1);
  
  netInitialize ();

  Init_Thread2();
//MSGQUEUE_OBJ_t_LCD t;
//snprintf(t.Buf, sizeof(t.Buf), "12:34:56");
//t.Idx = 0;
//osMessageQueuePut(mid_MsgQueue_LCD, &t, 0U, 0U);

//snprintf(t.Buf, sizeof(t.Buf), "03-10-2026");
//t.Idx = 1;
//osMessageQueuePut(mid_MsgQueue_LCD, &t, 0U, 0U);
  
   Tid_RTC = osThreadNew (RTC_Thread, NULL, NULL);
   TID_SNTP = osThreadNew (SNTP_Thread, NULL, NULL);
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  //  TID_Display = osThreadNew (Display,  NULL, NULL);
init_Leds();
Init_Timers();
  osThreadExit();
}
