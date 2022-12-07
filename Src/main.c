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
#include <string.h>
#include <stdbool.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpc21xx.h"

/* Peripheral includes. */
//#include "main.h"
#include "serial.h"
#include "GPIO.h"
#include "main.h"

/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

QueueHandle_t Queue = NULL;

int Task1_in, Task1_out, Total1
				  ,Task2_in, Task2_out, Total2
					,Task3_in, Task3_out, Total3
					,Task4_in, Task4_out, Total4
					,Task5_in, Task5_out, Total5
					,Task6_in, Task6_out, Total6;

int system_time, cpu_load ;


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
void vApplicationTickHook( void ){
	GPIO_write(PORT_0,PIN0,PIN_IS_HIGH);
	GPIO_write(PORT_0,PIN0,PIN_IS_LOW);
};

void vApplicationIdleHook( void ){
	GPIO_write(PORT_0,PIN3,PIN_IS_HIGH);
	GPIO_write(PORT_0,PIN3,PIN_IS_LOW);
};


static void prvSetupHardware( void );
/*-----------------------------------------------------------*/



void Button_1_Monitor (void * pvParameters ){
		
	  message_t message;
		bool current_state, old_state = GPIO_read(PORT_1,PIN0);
	  TickType_t xLastWakeTime = xTaskGetTickCount() ;
		
		vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
	  for(;;){
		current_state = GPIO_read(PORT_1,PIN0);
			
			if( current_state != old_state){
				
				if(current_state){
						strcpy((char *)(message.ucData), "Btn1_Rising");
				}
				else{
						strcpy((char *)(message.ucData), "Btn1_Falling");
				}
				xQueueSend(Queue, (void*)&message , 0);
			}
		
			vTaskDelayUntil(&xLastWakeTime,50);
		}
			
	
};


void Button_2_Monitor (void * pvParameters ){

		message_t message;
		bool current_state, old_state = GPIO_read(PORT_1,PIN1);
		TickType_t xLastWakeTime = xTaskGetTickCount() ;
		
		vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );
	  for(;;){
		current_state = GPIO_read(PORT_1,PIN1);
			
			if( current_state != old_state){
				
				if(current_state){
						strcpy((char *)(message.ucData), "Btn2_Rising");
				}
				else{
						strcpy((char *)(message.ucData), "Btn2_Falling");
				}
				xQueueSend(Queue, (void*)&message , 0);
			}
			
			vTaskDelayUntil(&xLastWakeTime,50);
		}
		
};

	
void Periodic_Transmitter (void * pvParameters ){

	message_t message;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	strcpy((char *)(message.ucData), "Hello\n");
	
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	for(;;){

		if (xQueueSend(Queue, (void*)&message, 5) == pdPASS){		
		}
		vTaskDelayUntil( &xLastWakeTime, 100 );
	}

};

void UART_Receiver (void * pvParameters ){
	
		TickType_t xLastWakeTime = xTaskGetTickCount();
		message_t message_buffer;
	
		vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
		for(;;){
			
				if(xQueueReceive( Queue, &(message_buffer), 0 ) == pdPASS){
				
					vSerialPutString((const signed char*) message_buffer.ucData, 7);
				}
      
	 
			vTaskDelayUntil( &xLastWakeTime, 20);
		}
};

void Load_1_Simulation (void * pvParameters ){
	
		TickType_t xLastWakeTime = xTaskGetTickCount();
		int i = 0;
	
		vTaskSetApplicationTaskTag( NULL, ( void * ) 5 );
		for(;;){

			for (i = 0; i < 33333; i++){
				i = i;
			}
			vTaskDelayUntil(&xLastWakeTime,10)
	}
		
};



void Load_2_Simulation (void * pvParameters ){
	
	  TickType_t xLastWakeTime = xTaskGetTickCount();
		int i = 0;
	
		vTaskSetApplicationTaskTag( NULL, ( void * ) 6 );
		for(;;){

			for (i = 0; i < 80000; i++){
				i = i;
			}
			vTaskDelayUntil(&xLastWakeTime,100)
	}
};
	


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */

int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	

	//Queue1 = xQueueCreate(5,sizeof(message_t));
		Queue = xQueueCreate(5,sizeof(message_t));
	
    /* Create Tasks here */
		/* Create the task, storing the handle. */
  
	xTaskPeriodicCreate( Button_1_Monitor , "Button_1_Monitor",100, ( void * ) 0,1, NULL, 50 );
	xTaskPeriodicCreate( Button_2_Monitor , "Button_2_Monitor",100, ( void * ) 0,1, NULL, 50 );
	
 	xTaskPeriodicCreate( Periodic_Transmitter , "Periodic_Transmitter", 100, ( void * ) 0, 1, NULL, 100 );
	xTaskPeriodicCreate( UART_Receiver, "UART_Receiver", 100, ( void * ) 0, 1, NULL, 20 );	

	xTaskPeriodicCreate( Load_1_Simulation ,"Load_1_Simulation",100, ( void * ) 0,1, NULL, 10 );
	xTaskPeriodicCreate( Load_2_Simulation , "Load_2_Simulation",100, ( void * ) 0,1, NULL, 100 );
										
										
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



/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
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
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


