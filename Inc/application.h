/**
  ******************************************************************************
  * @file           : application.h
  * @brief          : Header for application.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/


/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/**
  * @brief Macro definitions for LED control
  */  
#define LED_Toggle(led)					HAL_GPIO_TogglePin(GPIOE, led)
#define LED_Control(led, state) HAL_GPIO_WritePin(GPIOE, led, state)
#define LED_ON									GPIO_PIN_SET
#define LED_OFF									GPIO_PIN_RESET

#define USART_RX_BUFF_SIZE 128
extern uint8_t  usart_rx_buff[];

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __APPLICATION_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
