/**
  ******************************************************************************
  * @file           : application.cpp
  * @brief          : содержит код задач RTOS 
	*	@author					: rual
	* @date						:	18.07.2018 
  ******************************************************************************	
	*/
#include "application.hpp"

/// последний символ сообщения
#define SEPARATOR 0x00

/**
@brief идентификаторы ответов 
*/
typedef enum {
	TX_NULL,
	TX_ERROR,
	TX_BUSY,
	TX_OK,
	TX_ADC
} enTx_t;


xQueueHandle RxQueue; 								/// recieve byte queue
xQueueHandle RxMessageQueue; 					/// recieve message queue
xQueueHandle TxQueue; 								/// transmit queue
xQueueHandle AnalogFlowRateQueue; 		/// ADC and DAC rate control queue
SemaphoreHandle_t  xBlinkRxSemaphore; /// RX blinck semaphore

void USARTRXTask(void  * argument);
void RxParseTask(void  * argument);
void BlinkRxTask(void  * argument);
void ChangeAnalogFlowRateTask(void  * argument);
void USARTTxTask(void  * argument);


/**
*******************************************************************************
* @function init_application
* @brief 	: создание и запуск потоков и очередей приложения 
* @param 	: Нет
*	@return	: Нет
*******************************************************************************
*/
void init_application(void)
{
	/// Semaphore create 
	xBlinkRxSemaphore = xSemaphoreCreateBinary();
	
	/// Task create	
	xTaskCreate(USARTRXTask, "Uasrt Rx Task", 1024, NULL, osPriorityNormal, NULL);	
	
	xTaskCreate(USARTTxTask, "Transmit Task", 512, NULL, osPriorityNormal, NULL);	
	
	xTaskCreate(RxParseTask, "Reiseive Parsing Task", 512, NULL, osPriorityNormal, NULL);	
	
	xTaskCreate(BlinkRxTask, "Blink Rx Task", 128, NULL, osPriorityNormal, NULL);
	
	xTaskCreate(ChangeAnalogFlowRateTask, "Change Analog Flow Rate Task", 128, NULL, osPriorityNormal, NULL);
	
	
	/// Queue create
	RxQueue = xQueueCreate(RX_SIMB_QUEUE_SZ,1); 
	
	RxMessageQueue = xQueueCreate(RX_MESSAGE_QUEUE_SZ,32); 
	
	TxQueue = xQueueCreate(16,sizeof (enTx_t)); 
	
	AnalogFlowRateQueue = xQueueCreate(1,2); 	
}

/**
**********************************************************************
*	@function USARTRXTask
* @breef		выбирает принятые байты из очереди RxQueue
* 					и формирует очередь сообщений RxMessageQueue
**********************************************************************
*/
void USARTRXTask(void  * argument)
{ 
	uint8_t rx_char;
		
	uint8_t rx_message[32]; 
	
	uint8_t* pmessage = rx_message;
	
  /* Infinite loop */
  for(;;)
  {
		xQueueReceive(RxQueue, &rx_char, portMAX_DELAY);
				
		*(pmessage++) = rx_char;
		
		if (pmessage > &(rx_message[31])) 
			pmessage = rx_message;
				
    
		xSemaphoreGive(xBlinkRxSemaphore); 
		
		
		if (rx_char == SEPARATOR)
		{
			*(--pmessage) = 0;
			
			/// очередь сообщений переполнена 
			if (xQueueSend(RxMessageQueue, rx_message, 1) == pdFAIL)
			{
				enTx_t tx_event = TX_BUSY;
				
				xQueueSend(TxQueue, &tx_event, portMAX_DELAY); 
			}

			pmessage = rx_message;
		}		
  }
}

/// содержит среднее значение буфера аналогового потока
static float average = 0;

/**
**********************************************************************
*	@function USARTTxTask
* @breef		передает и формирует очередь сообщений 
**********************************************************************
*/
void USARTTxTask(void  * argument)
{ 
	enTx_t tx_event;
	
	uint8_t * pTxt = NULL;
	
	uint8_t   szTxt = 0;
	
	const char busy_text[]  = "BUSY\r\n";
	
	const char ok_text[]		= "OK\r\n";
	
	const char err_text[]		= "ERROR\r\n";
	
	char adc_text[16];
		
	
  /* Infinite loop */
  for(;;)
  {
		xQueueReceive(TxQueue, &tx_event, portMAX_DELAY);
		
		switch (tx_event) 
		{
			case TX_BUSY:	
				
				szTxt =  sizeof busy_text;
			
				pTxt = (uint8_t*) busy_text;	
			
			break;
			
			
			case TX_OK:	
				
				szTxt =  sizeof ok_text;
			
				pTxt = (uint8_t*) ok_text;	
			
			break;
				
			
			case TX_ERROR:	
				
				szTxt =  sizeof err_text;
			
				pTxt = (uint8_t*) err_text;	
			
			break;
			
				
			case TX_ADC:
				
				szTxt = sprintf(adc_text,"%f\r\n", average);
			
				pTxt = (uint8_t*) adc_text;	
			
			break;	
			

			default: szTxt = 0;	break;				
		}			
			
		
		if (szTxt) HAL_UART_Transmit_DMA(&huart1, pTxt, szTxt); 
		
		
    /// мигаем синим СВД при выполненни команды
		LED_Control(Blue_LED, LED_ON);
		
		osDelay(100);
		
		LED_Control(Blue_LED, LED_OFF);
  }
}
/******************************************************************** 
* @function USARTRXTask
* @breef		мигает светодиодом  при приеме байта по USART 
*
*********************************************************************/
void BlinkRxTask(void  * argument)
{
  /* Infinite loop */
  for(;;)
  {    
		xSemaphoreTake(xBlinkRxSemaphore,  portMAX_DELAY);
		
		
		LED_Control(Yellow_LED, LED_ON);
		
		osDelay(100);
		
		LED_Control(Yellow_LED, LED_OFF);
  }
}


/******************************************************************** 
* @function RxParseTask function 
* @brief 		разбирает очередь сообщений от USART 
*
*********************************************************************/
void RxParseTask(void  * argument)
{ 
	uint8_t tx_event;	

#ifdef	USE_STL	
	CommandParsing cp;
#endif	
	
	char rx_message[32]; 
	
  /* Infinite loop */
  for(;;)
  {    
		xQueueReceive(RxMessageQueue, rx_message, portMAX_DELAY);

#ifdef	USE_STL		
		string  str = (char*)rx_message;	
		
		switch(cp.GetCommand(str))
			
#else		
		switch(GetCommand(rx_message))			
#endif	
		{
			case CommandParsing::CMD_LED_ON: 
			
				LED_Control(Red_LED, LED_ON); 
				
				tx_event = TX_OK;
			
			break;
			
			
			case CommandParsing::CMD_LED_OFF: 
				
				LED_Control(Red_LED, LED_OFF); 
			
				tx_event = TX_OK;
			
			break;
			
			
			case CommandParsing::CMD_VOLTAGE: 
				
				tx_event = TX_ADC;
			
			break;
			
			
			case CommandParsing::CMD_RATE: 
				
				uint16_t rate;
			 
				if (sscanf(rx_message,"%*s%d",&rate) == 1
					&& rate >= 10 && rate <= 10000) 
				{
					xQueueSend(AnalogFlowRateQueue, &rate, 1); 
					
					tx_event = TX_OK;
					
					break;
				}
					
			
			
			default: 
			
				tx_event = TX_ERROR;
			
			break;
			
		} 
		
		if (tx_event) xQueueSend(TxQueue, &tx_event, 1);
		
		osDelay(1000); /// исскуственная задержка для загрузки очереди
  }
}


/**
***********************************************************
* @breaf ChangeAnalogFlowRateTask function								*
* задача выполняет смену скарости аналогового потока.			*
* при простое выполняет вычисление среднего значения 			*
* буфера за последнюю секунду 														*
***********************************************************/
void ChangeAnalogFlowRateTask(void  * argument)
{
	uint16_t rate = 1000;
	
	StartAnalogLoop(rate);
	
  /* Infinite loop */
  for(;;)
  {
		if(xQueueReceive(AnalogFlowRateQueue, &rate, 1) == pdPASS)
		{
			/// выполняем изменение скорости аналогового потока
			StopAnalogLoop();
			
			StartAnalogLoop(rate);		
			
			
			LED_Control(LD6_Pin, LED_ON);
			
			osDelay(100);
			
			LED_Control(LD6_Pin, LED_OFF);
		}	
			
		/// подсчет среднего значения аналогового потока 
		uint32_t summa = 0; 
		
		uint16_t j = rate;
		
		do	
			for(uint16_t i = 0; i < ANALOG_BUFF_SIZE; i++) 
			{
				summa += analog_flow_buf[i];
								
				if (j--) break;
			}
		while (j);
		
		average = (float)summa / rate / 4095 * 3.3f;    
  }  
}

/**
***************************************************************
* @brief : Окончание приема байта через USART 									
*	принятый байт заталкивается в первичную байтовую очередь 
***************************************************************
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
	BaseType_t state;
	
	xQueueSendFromISR(RxQueue, usart_rx_buff, &state); 	
	
	HAL_UART_Receive_IT(&huart1,usart_rx_buff,1);			/// вот это не есть ОПТИМАЛЬНО, лучше было просто забирать
																										///  значение DR в обработчике, но HAL не предполагает
																										/// простую настройку прерывания без софтового обвеса
}


