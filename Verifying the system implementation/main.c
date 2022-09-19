/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
 */


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

#define NULL_PTR                             (void*)0



TaskHandle_t Button_1_Monitor_handler      = NULL;
TaskHandle_t Button_2_Monitor_handler      = NULL;
TaskHandle_t PeriodicTransmitter_handler = NULL;
TaskHandle_t Uart_Receiver_handler        = NULL;
TaskHandle_t Load1_handler                = NULL;
TaskHandle_t Load2_handler     = NULL;

QueueHandle_t xQueue;


#if ( TIMER_TRACE_Config == 1 )

int button1_TaskInTime,      button1_TaskOutTime,     button1_TaskTotalTime;
int button2_TaskInTime,      button2_TaskOutTime,     button2_TaskTotalTime;
int Transmitter_TaskInTime,  Transmitter_TaskOutTime, Transmitter_TaskTotalTime;
int Consumer_TaskInTime,     Consumer_TaskOutTime,    Consumer_TaskTotalTime;
int load1_TaskInTime,        load1_TaskOutTime,       load1_TaskTotalTime;
int load2_TaskInTime,        load2_TaskOutTime,       load2_TaskTotalTime;
/* SYSTEM_TIME = TITC */ 
int system_Time = 0;
int cpu_Load = 0;
#endif
/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* Task to be created. */

/*Task 1: ""Button_1_Monitor"", {Periodicity: 50, Deadline: 50}
This task will monitor rising and falling edge on button 1 and send this event to the consumer task. (Note: The rising and failling edges are treated as separate events, hence they have separate strings) 
*/ 

void Button_1_Monitor( void * pvParameters )
{
		
	/* Intial Value */ 
  pinState_t B1_prev_state = PIN_IS_HIGH;  
		
	/* Storing New Value */ 
	pinState_t B1_curr_state;      
    
	/*Pointer Holding Message_String */ 	
	char* msg1_ptr = NULL_PTR;        

	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
#if ( TIMER_TRACE_Config == 1 )
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
#endif

	for( ;; )
	{
		B1_curr_state = GPIO_read(PORT_0, PIN1);

     /* Edge Checking */ 
		if ( B1_curr_state != B1_prev_state )
		{

			/* Case 1  */
			if ( B1_curr_state == PIN_IS_HIGH )
			{
								
				/* Queue Empty or Not */
        if ( xQueue != 0 )
				{
					msg1_ptr = "Button1RisingEdge\n";
					xQueueSend( xQueue, ( void * ) &msg1_ptr, ( TickType_t ) 0 );
				}
			}
			
			else if ( B1_curr_state == PIN_IS_LOW )
			{
				/* check if the Queue have a place or not if yes store the message in it
							     and dont't wait any ticks for a place to be empty in the Queue */
				if ( xQueue != 0 )
				{
					msg1_ptr = "Button1FallingEdge\n";
					xQueueSend( xQueue, ( void * ) &msg1_ptr, ( TickType_t ) 0 );
				}
			}

		}
		/* Updating State */
		B1_prev_state = B1_curr_state;

     /* GPIO configuration For Logic_Analyzer */ 
			vTaskDelayUntil( &xLastWakeTime, 50 );
	


	
			GPIO_write (PORT_0, PIN9, PIN_IS_LOW);
	
	}
}

/*Task 2: ""Button_2_Monitor"", {Periodicity: 50, Deadline: 50}
This task will monitor rising and falling edge on button 2 and send this event to the consumer task. (Note: The rising and failling edges are treated as separate events, hence they have separate strings)
*/
void Button_2_Monitor( void * pvParameters )
{
	pinState_t B2_prev_state = PIN_IS_HIGH;  
	pinState_t B2_curr_state;                
	char* msg2_ptr = NULL_PTR;           

	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

#if ( TIMER_TRACE_Config == 1 )
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );
#endif

	for( ;; )
	{
		B2_curr_state = GPIO_read(PORT_0, PIN2);

		if ( B2_curr_state != B2_prev_state )
		{
			
			if ( B2_curr_state == PIN_IS_HIGH )
			{
				
				if ( xQueue != 0 )
				{
					msg2_ptr = "Button2RisingEdge\n";
					xQueueSend( xQueue, ( void * ) &msg2_ptr, ( TickType_t ) 0 );
				}
			}
			
			else if ( B2_curr_state == PIN_IS_LOW )
			{
		
				if ( xQueue != 0 )
				{
					msg2_ptr = "BUTTON2FALLINGEDGE\n";
					xQueueSend( xQueue, ( void * ) &msg2_ptr, ( TickType_t ) 0 );
				}
			}

		}
		B2_prev_state = B2_curr_state;

			vTaskDelayUntil( &xLastWakeTime, 50 );
		


			/* Trace the idle Task */
			GPIO_write (PORT_0, PIN9, PIN_IS_LOW);
		
	}
}

/*Task 3: ""Periodic_Transmitter"", {Periodicity: 100, Deadline: 100}
This task will send preiodic string every 100ms to the consumer task
*/
void Periodic_Transmitter( void * pvParameters )
{
	char* periodic_message = NULL_PTR;   
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();


#if (TIMER_TRACE_Config == 1 )
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
#endif

	for( ;; )
	{
		if ( xQueue != 0 )
		{
			periodic_message = "PERIODIC_TRANSMITTER\n";
			xQueueSend( xQueue, ( void * ) &periodic_message, ( TickType_t ) 0 );
		}
		
		
		vTaskDelayUntil( &xLastWakeTime, 100 );
		


			/*Trace the idle Task*/
			GPIO_write (PORT_0, PIN9, PIN_IS_LOW);
		
	}
}

/*Task 4: ""Uart_Receiver"", {Periodicity: 20, Deadline: 20}
This is the consumer task which will write on UART any received string from other tasks"
*/
void Uart_Receiver( void * pvParameters )
{   
	char* msg_ptr = NULL_PTR;   
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
#if (TIMER_TRACE_Config == 1 )
	vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
#endif
	
	for( ;; )
	{
		
		if( xQueueReceive( xQueue,&(msg_ptr ),( TickType_t ) 0 ) == pdPASS )
		{
			vSerialPutString((const signed char *)(msg_ptr), 22);
		}

			vTaskDelayUntil( &xLastWakeTime, 20 );
		


			/* Trace the idle task */
			GPIO_write (PORT_0, PIN9, PIN_IS_LOW);
		
	}
}

/*Task 5: ""Load_1_Simulation"", {Periodicity: 10, Deadline: 10}, Execution time: 5ms */
void Load_1_Simulation( void * pvParameters )
{
	uint16_t i;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();


#if ( TIMER_TRACE_Config == 1 )
	vTaskSetApplicationTaskTag( NULL, ( void * ) 5);
#endif

	for( ;; )
	{

		for ( i=0; i < 30000; i++ );

			vTaskDelayUntil( &xLastWakeTime, 10 );
		


			/* Trace the idle task */
			GPIO_write (PORT_0, PIN9, PIN_IS_LOW);
	}
}

/*Task 6: ""Load_2_Simulation"", {Periodicity: 100, Deadline: 100}, Execution time: 12ms*/
void Load_2_Simulation( void * pvParameters )
{
	uint32_t i;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	
#if (TIMER_TRACE_Config == 1 )
	vTaskSetApplicationTaskTag( NULL, ( void * ) 6 );
#endif


	for( ;; )
	{
		for ( i = 0; i < 90500; i++);

			vTaskDelayUntil( &xLastWakeTime, 100 );


			/* Trace the idle task */
			GPIO_write (PORT_0, PIN9, PIN_IS_LOW);
	
	}
}

/* Tick_Hook CallBack Function */ 
void vApplicationTickHook( void )
{
	GPIO_write (PORT_0, PIN0, PIN_IS_HIGH);
	GPIO_write (PORT_0, PIN0, PIN_IS_LOW);
}

/*Idle_Task CallBack Function */ 
void vApplicationIdleHook( void )
{
	GPIO_write (PORT_0, PIN9, PIN_IS_HIGH);
}

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	xQueue = xQueueCreate( 3, sizeof( char* ) );


	xTaskPeriodicCreate(
			Button_1_Monitor,                  /* Function that implements the task. */
			"BUTTON 1 MONITOR",                /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Button_1_Monitor_handler,         /* Used to pass out the created task's handle. */
			50);                               /* Period for the task */

	xTaskPeriodicCreate(
			Button_2_Monitor,                  /* Function that implements the task. */
			"BUTTON 2 MONITOR",                /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Button_2_Monitor_handler,         /* Used to pass out the created task's handle. */
			50);                               /* Period for the task */

	xTaskPeriodicCreate(
			Periodic_Transmitter,               /* Function that implements the task. */
			"PERIODIC TRANSMITTER",             /* Text name for the task. */
			100,                                /* Stack size in words, not bytes. */
			( void * ) 0,                       /* Parameter passed into the task. */
			1,                                  /* Priority at which the task is created. */
			&PeriodicTransmitter_handler,       /* Used to pass out the created task's handle. */
			100);                               /* Period for the task */

	xTaskPeriodicCreate(
			Uart_Receiver,                      /* Function that implements the task. */
			"UART RECEIVER",                    /* Text name for the task. */
			100,                                /* Stack size in words, not bytes. */
			( void * ) 0,                       /* Parameter passed into the task. */
			1,                                  /* Priority at which the task is created. */
			&Uart_Receiver_handler,             /* Used to pass out the created task's handle. */
			20);                                /* Period for the task */

	xTaskPeriodicCreate(
			Load_1_Simulation,                 /* Function that implements the task. */
			"LOAD 1 SIMULATION",               /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Load1_handler,                    /* Used to pass out the created task's handle. */
			10);	                            /* Period for the task */

	
	xTaskPeriodicCreate(
			Load_2_Simulation,                 /* Function that implements the task. */
			"LOAD 2 SIMULATION",               /* Text name for the task. */
			100,                               /* Stack size in words, not bytes. */
			( void * ) 0,                      /* Parameter passed into the task. */
			1,                                 /* Priority at which the task is created. */
			&Load2_handler,                    /* Used to pass out the created task's handle. */
			100); 	                           /* Period for the task */

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

static void ConfigTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();

	/* Configure trace timer 1 and read TITC to get current tick */
	ConfigTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


