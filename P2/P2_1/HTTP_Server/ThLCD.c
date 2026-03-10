#include "LCD.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 

extern uint16_t positionL1;
extern uint16_t positionL2;
osThreadId_t tid_Thread2;                        // thread id
 

 
 
int Init_Thread2 (void) {
 
  tid_Thread2 = osThreadNew(Thread2, NULL, NULL);
  if (tid_Thread2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 



MSGQUEUE_OBJ_t_LCD msg_recibido;


osStatus_t status;

void Pintar(MSGQUEUE_OBJ_t_LCD msg);

void Thread2 (void *argument) {
  LCD_reset();
	LCD_init();
	LCD_Clean();
  while (1) {
    status = osMessageQueueGet(mid_MsgQueue_LCD, &msg_recibido, NULL, osWaitForever);
		if(status == osOK){
			Pintar(msg_recibido);
		}	
		osThreadYield(); 
  }
}

//void Pintar(MSGQUEUE_OBJ_t_LCD msg){
//	int fin = 0;
//	positionL1 = 0;
//	positionL2 = 0;
//	if(msg.Idx == 0){
//		for(int i = 0; i<128; i++){
//			if(msg.Buf[i]!=0x00 && fin!= 1){
//				symbolToLocalBuffer_L1(msg.Buf[i]);
//				if(msg.Buf[i+1]==0x00){
//					fin = 1;
//				}
//			}
//		}
//		LCD_update();
//	}else{
//		for(int i = 0; i<128; i++){
//			if(msg.Buf[i]!=0x00 && fin!= 1){
//				symbolToLocalBuffer_L2(msg.Buf[i]);
//				if(msg.Buf[i+1]==0x00){
//					fin = 1;
//				}
//			}
//		}
//		LCD_update();
//	}
//}

void Pintar(MSGQUEUE_OBJ_t_LCD msg){
  positionL1 = 0;
  positionL2 = 0;

  if (msg.Idx == 0) {
    for (int i = 0; i < 128 && msg.Buf[i] != '\0'; i++) {
      symbolToLocalBuffer_L1((uint8_t)msg.Buf[i]);
    }
  } else {
    for (int i = 0; i < 128 && msg.Buf[i] != '\0'; i++) {
      symbolToLocalBuffer_L2((uint8_t)msg.Buf[i]);
    }
  }

  LCD_update();
}