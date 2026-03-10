
 #include "TH_LED.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
 osThreadId_t tid_ThLeds;                        // thread id
 
static GPIO_InitTypeDef GPIO_InitStruct;


int Init_ThLeds (void) {
 
  tid_ThLeds = osThreadNew(ThLeds, NULL, NULL);
  if (tid_ThLeds == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThLeds (void *argument) {

initLeds();
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
  while (1) {
     // Insert thread code here...
     osThreadFlagsWait(0x04,osFlagsWaitAny, osWaitForever);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    osDelay(500);
     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

    osThreadYield();                            // suspend thread
  }
}




void initLeds(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
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



