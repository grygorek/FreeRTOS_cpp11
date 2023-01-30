/*
 * FreeRTOS Kernel V10.2.1
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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

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

/*
 * The FreeRTOS Cortex-A port implements a full interrupt nesting model.
 *
 * Interrupts that are assigned a priority at or below
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which counter-intuitively in the ARM
 * generic interrupt controller [GIC] means a priority that has a numerical
 * value above configMAX_API_CALL_INTERRUPT_PRIORITY) can call FreeRTOS safe API
 * functions and will nest.
 *
 * Interrupts that are assigned a priority above
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which in the GIC means a numerical
 * value below configMAX_API_CALL_INTERRUPT_PRIORITY) cannot call any FreeRTOS
 * API functions, will nest, and will not be masked by FreeRTOS critical
 * sections (although it is necessary for interrupts to be globally disabled
 * extremely briefly as the interrupt mask is updated in the GIC).
 *
 * FreeRTOS functions that can be called from an interrupt are those that end in
 * "FromISR".  FreeRTOS maintains a separate interrupt safe API to enable
 * interrupt entry to be shorter, faster, simpler and smaller.
 *
 */
#define configMAX_API_CALL_INTERRUPT_PRIORITY	18

uint32_t SystemCoreClockFreq();

#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1
#define configUSE_TICKLESS_IDLE					0
#define configTICK_RATE_HZ						( ( TickType_t ) 1000 )
#define configUSE_PREEMPTION					1
#define configUSE_IDLE_HOOK						0
#define configUSE_TICK_HOOK						0
#define configMAX_PRIORITIES					( 7 )
#define configMINIMAL_STACK_SIZE				( ( unsigned short ) 250 ) /* Large in case configUSE_TASK_FPU_SUPPORT is 2 in which case all tasks have an FPU context. */
#define configTOTAL_HEAP_SIZE					( 125 * 1024 )
#define configMAX_TASK_NAME_LEN					( 10 )
#define configUSE_TRACE_FACILITY				0
#define configUSE_16_BIT_TICKS					0
#define configIDLE_SHOULD_YIELD					1
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE				8
#define configCHECK_FOR_STACK_OVERFLOW			2
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_MALLOC_FAILED_HOOK			1
#define configUSE_APPLICATION_TASK_TAG			0
#define configUSE_COUNTING_SEMAPHORES			1
#define configUSE_QUEUE_SETS					1
#define configSUPPORT_STATIC_ALLOCATION			0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5

#define configMAIN_STACK_SIZE 384 // in words (bytes = x4)

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 					0
#define configMAX_CO_ROUTINE_PRIORITIES 		( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS						0
#define configTIMER_TASK_PRIORITY				( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH				5
#define configTIMER_TASK_STACK_DEPTH			( configMINIMAL_STACK_SIZE * 2 )

/* If configUSE_TASK_FPU_SUPPORT is set to 1 (or undefined) then each task will
be created without an FPU context, and a task must call vTaskUsesFPU() before
making use of any FPU registers.  If configUSE_TASK_FPU_SUPPORT is set to 2 then
tasks are created with an FPU context by default, and calling vTaskUsesFPU() has
no effect. */
#define configUSE_TASK_FPU_SUPPORT				1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskCleanUpResources 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTimerPendFunctionCall 0
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xTaskAbortDelay 1
#define INCLUDE_xTaskGetTaskHandle 1
#define INCLUDE_xTaskGetHandle 1
#define INCLUDE_xSemaphoreGetMutexHolder 1

#define configGENERATE_RUN_TIME_STATS 0

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) while(1);

/* If configTASK_RETURN_ADDRESS is not defined then a task that attempts to
return from its implementing function will end up in a "task exit error"
function - which contains a call to configASSERT().  However this can give GCC
some problems when it tries to unwind the stack, as the exit error function has
nothing to return to.  To avoid this define configTASK_RETURN_ADDRESS to 0.  */
#define configTASK_RETURN_ADDRESS	NULL


/****** ARM Cortex A9 Hardware specific settings. *******************************************/

void vSetupTickInterrupt(void);
#define configSETUP_TICK_INTERRUPT() vSetupTickInterrupt()

void vClearTickInterrupt(void);
#define configCLEAR_TICK_INTERRUPT() vClearTickInterrupt()

#define CA9_PERIPH_BASE (0x1E000000UL)        /* call __get_CBAR() to get this value*/
#define CA9_IRQ_DISTRIBUTOR_OFFSET (0x1000UL) /* https://developer.arm.com/documentation/ddi0407/g/Introduction/Private-Memory-Region */
#define CA9_IRQ_GIC_OFFSET (0x100UL)          /* https://developer.arm.com/documentation/ddi0407/g/Introduction/Private-Memory-Region */
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS (CA9_PERIPH_BASE + CA9_IRQ_DISTRIBUTOR_OFFSET)        
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET (CA9_IRQ_GIC_OFFSET - CA9_IRQ_DISTRIBUTOR_OFFSET)     
#define configUNIQUE_INTERRUPT_PRIORITIES 32

#ifndef pdTICKS_TO_MS
#define pdTICKS_TO_MS(ticks) \
  ((((long long)(ticks)) * (configTICK_RATE_HZ)) / 1000)
#endif

#endif /* FREERTOS_CONFIG_H */

