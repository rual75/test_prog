/**
  ******************************************************************************
  * @file           : application.hpp
  * @brief          : 	содержит класс, использующий STL, и функцию 
  * 					дл€ распозновани€ команды в строке 
  *	@author			: rual
  * @date			:	18.07.2018 
  ******************************************************************************
*/

#ifndef __PROCESS_HPP_
#define __PROCESS_HPP_

#include "main.h"
#include "cmsis_os.h"
#include "stm32f3xx_hal.h"

#include <strstream>
#include <string>
#include <map>
#include <string.h>

using namespace std;

/**
* дл€ использоваани€ класса map библиотеки STL дл€ дешифровки команды 		*
* раскоментируйте #define USE_STL, код рабочий и компилируетс€ корректно,	*
* но на платформе STM32F3Discovery не запускаетс€ из-за недостатка RAM 		*
* ¬ рабочей версии программы применена стандартна€ библиотека string.h		*
*	и более легка€ функци€ strcmp()
*/
//#define USE_STL 

/// прототип глобальной струтуры USART1
extern UART_HandleTypeDef huart1;
/**
**********************************************************************
 @class 	CommandParsing выполн€ет распознавание команды
 @param 	char* rx_message стандартна€ строка, содержаща€ команду
 @return 	одно из значений enCmd_t, соответсвующа€ команде, либо, 
			если команда не распознана - CMD_NULL
*/

class CommandParsing {
	
	
	public:
		/**
		значение команды */
		typedef enum {
			CMD_NULL,
			CMD_VOLTAGE,
			CMD_LED_ON,
			CMD_LED_OFF,
			CMD_RATE
		} enCmd_t;
		
		/**
		конструктор - заполн€ет ключи и значени€ команд
		*/
		CommandParsing(void) 
		{
			command["LED_ON"] 	=	CMD_LED_ON;
			command["LED_OFF"]	=	CMD_LED_OFF;
			command["GET_ADC_AVG_VOLTAGE"]	=	CMD_VOLTAGE;
			command["SET_ADC_SAMPLE_RATE"]	=	CMD_RATE; 
		}
		
		/**
		метод выполн€ющий распознование команды в строке
		@param 	string key - строка с ключом-строкой
		@return возращает значение команды, если распознана, CMD_NULL - если нет. 
		*/
		enCmd_t GetCommand(std::string key) 
		{
			return command[key];
		}
		
	private:
		/// объект STL map - содержит соответсвие ключа-строки команды и значение
		std::map <std::string , enCmd_t> command;
		
};

/**
**********************************************************************
 @function 	GetCommand выполн€ет распознавание команды
 @param 	char* rx_message стандартна€ строка, содержаща€ команду
 @return 	одно из значений enCmd_t, соответсвующа€ команде, либо, 
			если команда не распознана - CMD_NULL
*/

CommandParsing::enCmd_t GetCommand(char* rx_message)
{
	if 		(strcmp(rx_message,"LED_ON")	== 0)	return CommandParsing::CMD_LED_ON;
	
	else if (strcmp(rx_message,"LED_OFF")	== 0)	return CommandParsing::CMD_LED_OFF;
	
	else if (strcmp(rx_message,"GET_ADC_AVG_VOLTAGE") == 0)	return  CommandParsing::CMD_VOLTAGE;
	
	else if (strncmp(rx_message,"SET_ADC_SAMPLE_RATE",18) == 0)	return  CommandParsing::CMD_RATE;
	
	return CommandParsing::CMD_NULL;	
}

#endif // __PROCESS_HPP_

