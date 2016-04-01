/*
 * BJ Devices Travel Box series midi controller library
 * @file	log.h
 * 
 * @brief	UART based simple logger 				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */


#ifndef LOG_H_
#define LOG_H_

#include "uart.h"
#include <stdio.h>
#include <avr/pgmspace.h>

#define MAX_LOG_LENGTH 128

typedef enum
{
	SEV_TRACE = 0
	,SEV_INFO
	,SEV_WARNING
	,SEV_ERROR
}LogSeverity;

static const char stringSevTrace[] PROGMEM = "TRACE: ";
static const char stringSevInfo[] PROGMEM = "INFO: ";
static const char stringSevWarning[] PROGMEM = "WARNING: ";
static const char stringSevError[] PROGMEM = "ERROR: ";

static PGM_P const stringSeverities[] PROGMEM =
{
	stringSevTrace,
	stringSevInfo,
	stringSevWarning,
	stringSevError,
};

#ifdef LOG_ENABLED

#	ifndef LOG_SEVERITY
#		warning "LOG_SEVERITY not defined for log.h"
#		define LOG_SEVERITY 0
#	endif

#	define LOG(severity,format,args...)								\
			do{														\
				if (severity >= LOG_SEVERITY)						\
				{													\
					uart1PutStringP(stringSeverities[severity]);	\
					char message[MAX_LOG_LENGTH];					\
					sprintf(message, format, ## args);				\
					uart1PutString(message);						\
					uart1PutString("\n\r");							\
				}													\
			}while(0)												\

#	else

#	define LOG(severity,format,args...)		\
		do{} while(0)						\

#	endif //LOG_ENABLED			

#endif /* LOG_H_ */