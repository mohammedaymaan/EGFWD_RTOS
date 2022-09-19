/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>
#include "GPIO.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/               

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 90 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN  	( 8 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		0
#define configUSE_TIME_SLICING           1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configUSE_MUTEXES                1
#define configUSE_APPLICATION_TASK_TAG   1

#define configUSE_EDF_SCHEDULER   1

#define TIMER_TRACE_Config 1


#if ( TIMER_TRACE_Config == 1 )	

extern int button1_TaskInTime,  button1_TaskOutTime,  button1_TaskTotalTime;
extern int button2_TaskInTime,  button2_TaskOutTime,  button2_TaskTotalTime;
extern int Transmitter_TaskInTime, Transmitter_TaskOutTime, Transmitter_TaskTotalTime;
extern int Consumer_TaskInTime,     Consumer_TaskOutTime,     Consumer_TaskTotalTime;
extern int load1_TaskInTime,    load1_TaskOutTime,    load1_TaskTotalTime;
extern int load2_TaskInTime,    load2_TaskOutTime,    load2_TaskTotalTime;

extern int system_Time;
extern int cpu_Load;

#define traceTASK_SWITCHED_OUT()	if ((int)pxCurrentTCB->pxTaskTag == 1)\
		{\
	         button1_TaskOutTime = T1TC;\
	         button1_TaskTotalTime += (button1_TaskOutTime - button1_TaskInTime);\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 2)\
		{\
			button2_TaskOutTime = T1TC;\
			button2_TaskTotalTime += (button2_TaskOutTime - button2_TaskInTime);\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 3)\
		{\
			Transmitter_TaskOutTime = T1TC;\
			Transmitter_TaskTotalTime += (Transmitter_TaskOutTime - Transmitter_TaskInTime);\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 4)\
		{\
			Consumer_TaskOutTime = T1TC;\
			Consumer_TaskTotalTime += (Consumer_TaskOutTime - Consumer_TaskInTime);\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 5)\
		{\
			load1_TaskOutTime = T1TC;\
			load1_TaskTotalTime += (load1_TaskOutTime - load1_TaskInTime);\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 6)\
		{\
			load2_TaskOutTime = T1TC;\
			load2_TaskTotalTime += (load2_TaskOutTime - load2_TaskInTime);\
		}\
		system_Time = T1TC;\
		cpu_Load = ((  button1_TaskTotalTime + button2_TaskTotalTime\
				+ Transmitter_TaskTotalTime + Consumer_TaskTotalTime\
				+ load1_TaskTotalTime +load2_TaskTotalTime ) / (float)system_Time ) * 100;

#define traceTASK_SWITCHED_IN()	  if ((int)pxCurrentTCB->pxTaskTag == 1)\
		{\
	        button1_TaskInTime = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 2)\
		{\
			button2_TaskInTime = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 3)\
		{\
			Transmitter_TaskInTime = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 4)\
		{\
			Consumer_TaskInTime = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 5)\
		{\
			load1_TaskInTime = T1TC;\
		}\
		else if ((int)pxCurrentTCB->pxTaskTag == 6)\
		{\
			load2_TaskInTime = T1TC;\
		}

#endif

#define configQUEUE_REGISTRY_SIZE 0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		      0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		  1
#define INCLUDE_uxTaskPriorityGet	      1
#define INCLUDE_vTaskDelete				  1
#define INCLUDE_vTaskCleanUpResources	  0
#define INCLUDE_vTaskSuspend			  1
#define INCLUDE_vTaskDelayUntil		   	  1
#define INCLUDE_vTaskDelay				  1



#endif /* FREERTOS_CONFIG_H */
