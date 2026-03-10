/**
  ******************************************************************************
  * @file    RTC/RTC_Calendar/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32F4xx RTC HAL API to configure
  *          Time and Date.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "rtc.h"
#include "LCD.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup RTC_Calendar
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t ToggleR,ToggleV = 0;
/* RTC handler declaration */
 RTC_HandleTypeDef RtcHandle;
  char rtc_text[2][20+1] = {0};
  char textweb[2][40];
  MSGQUEUE_OBJ_t_LCD lcd_rtc;
  
/* Buffers used for displaying Time and Date */
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;

extern osMessageQueueId_t mid_MsgQueue_LCD;
  
RTC_TimeTypeDef stimestructureget;
RTC_DateTypeDef sdatestructureget;

/* Private function prototypes -----------------------------------------------*/
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);
void ThRtc (void *argument);
  
void Rtc_Alarm_IRQHandler(void);
void RTC_Alarm_Config(void);
extern  osThreadId_t Tid_RTC;

const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 0, 216, 239, 35, 0 };

/****ID DE LOS TIMERS***/
osTimerId_t timer_id_1m;
osTimerId_t timer_ldR;
osTimerId_t timer_ldV;


/*************CALLBACK DE LOS TIMERS***********/

void Timer_Callback_ldR(void *arg)
{
  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
  ToggleR++;
  if(ToggleR > 4)
  {
    ToggleR = 0;
    osTimerStop(timer_ldR);
  }
}

void Timer_Callback_ldV(void *arg)
{
  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
  ToggleV++;
  if(ToggleV > 5)
  {
    ToggleR = 0;
    osTimerStop(timer_ldV);
  }
}

void Timer_Callback_1m(void *arg)
{
  osThreadFlagsSet(Tid_RTC,0x0C);
}


void RTC_Init(void)
{
//  typedef struct {
//RTC_TypeDef *Instance; /* Register base address */
//RTC_InitTypeDef Init; /* RTC required parameters */
//HAL_LockTypeDef Lock; /* RTC locking object */
//__IO HAL_RTCStateTypeDef State; /* Time communication state */
//} RTC_HandleTypeDef;
  
//  typedef struct {
//uint32_t HourFormat; /* Specifies the RTC Hour Format. */
//uint32_t AsynchPrediv; /* Specifies the RTC Asynchronous Predivider value. */
//uint32_t SynchPrediv; /* Specifies the RTC Synchronous Predivider value. */
//uint32_t OutPut; /* Specifies which signal will be routed to the RTC output. */
//uint32_t OutPutPolarity; /* Specifies the polarity of the output signal. */
//uint32_t OutPutType; /* Specifies the RTC Output Pin mode. */
//} RTC_InitTypeDef;
//}  
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  
  HAL_RTC_MspInit(&RtcHandle);
  
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
  }
}

void RTC_CalendarConfig(void){

    /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
  }
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
  }
   /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
  RTC_Alarm_Config();
  osTimerStart(timer_id_1m,1000);
  
  //Añadir timer para e4
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
 void RTC_CalendarShow(void)
{

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  
  /* Display time Format : hh:mm:ss */
osDelay(50);
 snprintf((char*)lcd_rtc.Buf, sizeof(lcd_rtc.Buf), "%02d:%02d:%02d",
         stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
     lcd_rtc.Idx = 0;
  osMessageQueuePut(mid_MsgQueue_LCD,&lcd_rtc,0U,0U);
  
  /* Display date Format : mm-dd-yy */
  osDelay(50);
  snprintf((char*)lcd_rtc.Buf, sizeof(lcd_rtc.Buf), "%02d-%02d-%04d",
         sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
     lcd_rtc.Idx = 1;
  osMessageQueuePut(mid_MsgQueue_LCD,&lcd_rtc,0U,0U);
}
/**
  * @brief RTC MSP Initialization 
  *        This function configures the hardware resources used in this example
  * @param hrtc RTC handle pointer
  * 
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
  *        the RTC clock source; in this case the Backup domain will be reset in  
  *        order to modify the RTC Clock source, as consequence RTC registers (including 
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *             
  * @retval None
  */

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    //Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    //Error_Handler();
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}
/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();

  /*##-2- Disables the PWR Clock and Disables access to the backup domain ###################################*/
  HAL_PWR_DisableBkUpAccess();
  __HAL_RCC_PWR_CLK_DISABLE();
  
}

/*        INICIALIZACION DE TODOS LOS LEDS        */

void init_Leds (void)
{
  
  static GPIO_InitTypeDef GPIO_InitStruct;
  //LEDS DE LA MPBOARD
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  //AZUL
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
  
  //VERDE
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
  
  //ROJO
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
  
  //LEDS DE LA PALCA STM
    //AZUL
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  
  //VERDE
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  
  //ROJO
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  
}


/*  INICIALIZACION DE LOS TIMERS Y FUNCIOLADIDAD  */

void Init_Timers(void)
{
  timer_id_1m = osTimerNew(Timer_Callback_1m,osTimerPeriodic,0,NULL);
  timer_ldR = osTimerNew(Timer_Callback_ldR,osTimerPeriodic,0,NULL);
  timer_ldV = osTimerNew(Timer_Callback_ldV,osTimerPeriodic,0,NULL);
}
void Toggle_ldV(void)
{
  osTimerStart(timer_ldV,500);
}

//void Toggle_ldR(void)
//{
//  osTimerStart(timer_ldR,200);
//}
/*                      ALARMA                   */
void RTC_Alarm_Config(void){
	RTC_AlarmTypeDef sAlarm = {0};
	uint8_t horas, minutos;
	HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
	
	if(stimestructureget.Minutes == 59){
		horas = stimestructureget.Hours + 1;
		minutos = 0;
	}else{
		horas = stimestructureget.Hours;
		minutos = stimestructureget.Minutes + 1;
	}
	
	sAlarm.Alarm = RTC_ALARM_A;
  sAlarm.AlarmTime.Hours = horas;
  sAlarm.AlarmTime.Minutes = minutos;
	sAlarm.AlarmTime.Seconds = 0;
	sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
       
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
	HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarm, RTC_FORMAT_BIN);
	
	CLEAR_BIT(RtcHandle.Instance->CR,RTC_CR_ALRAIE);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
    // Tu código de manejo de la interrupción aquí
    // Puedes realizar acciones específicas cuando se dispare la interrupcion
			osThreadFlagsSet (Tid_RTC,0x12);
}

void RTC_Alarm_IRQHandler(void){
	HAL_RTC_AlarmIRQHandler(&RtcHandle);
}
void TextoWeb(void){
	  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	
	sprintf(textweb[0], "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
	sprintf(textweb[1], "%02d-%02d-%04d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
}

/*-------------------------------SNTP--------------------------------------*/
/*-------------------------------------------------------------------------*/
	
//struct tm  ts;

//void get_time (void) {
////  if (netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback) == netOK) {
////    //printf ("SNTP request sent.\n");
////  }
////  else {
////    //printf ("SNTP not ready or bad parameters.\n");
////  }
//}
//static void time_callback (uint32_t seconds, uint32_t seconds_fraction){
//	
//  if (seconds == 0) {
//    //printf ("Server not responding or bad response.\n");
//  }
//	else{
//    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
//    ts = *localtime(&seconds);
//		/*##-1- Configure the Date #################################################*/
//		sdatestructure.Year = ts.tm_year;
//		if(ts.tm_mon==12){
//			sdatestructure.Month = 1;
//		}else{
//		sdatestructure.Month = ts.tm_mon+1;
//		}
//		
//		sdatestructure.WeekDay = ts.tm_wday + 1;
//    
//		/*##-2- Configure the Time #################################################*/
//		if(ts.tm_hour+1==23){
//			stimestructure.Hours = 0;
//      sdatestructure.Date = ts.tm_mday + 1;
//		}else if(ts.tm_hour==0){
//			stimestructure.Hours = 1;
//		}else{
//		  stimestructure.Hours = ts.tm_hour+2;
//      sdatestructure.Date = ts.tm_mday;
//		}
//		stimestructure.Minutes = ts.tm_min;
//		stimestructure.Seconds = ts.tm_sec;
//		stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
//    
//    HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BCD);
//		HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
//		RTC_Alarm_Config();
//  }
//}
  
///*---------------RESETEA EL RTC TRAS PULSAR EL BOTON-----------------------*/
///*-------------------------------------------------------------------------*/

//void reset_tiempo(void){
//		/*##-1- Configure the Date #################################################*/
//		sdatestructure.Year = 0;
//		sdatestructure.Month = 1;
//		sdatestructure.Date = 1;
//		sdatestructure.WeekDay = ts.tm_wday;
//		HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BCD);
//		/*##-2- Configure the Time #################################################*/
//  stimestructure.Hours = 0x02;
//  stimestructure.Minutes = 0x00;
//  stimestructure.Seconds = 0x00;
//		stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
//		HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
//		RTC_Alarm_Config();
//}