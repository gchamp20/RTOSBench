#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * OS specific imports
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/**
 * Parameters
 */
#define BASE_PRIO 10

/**
 * Type definitions
 */
typedef void no_task_retval_t;
typedef void* no_task_argument_t;
typedef int no_main_retval_t;
typedef int no_main_argument_t;
typedef no_task_retval_t (*no_task_entry_t)(no_task_argument_t);
typedef TaskHandle_t no_task_handle_t;
typedef long no_time_t;
typedef SemaphoreHandle_t no_sem_t;
typedef SemaphoreHandle_t no_mutex_t;
typedef EventGroupHandle_t no_event_t;
typedef QueueHandle_t no_mq_t;
typedef void (*no_int_handler_t)(int, void*);

extern volatile unsigned int* coreMailboxInterruptClr;

#define NO_DECLARE_INT_HANDLER(NAME, TIME_CNTR_SUFFIX) \
void NAME(int nIRQ, void *pParam) { \
	WRITE_T2_COUNTER(TIME_CNTR_SUFFIX) \
	*coreMailboxInterruptClr = 0x1; \
}

/**
 * Macros
 */
#define INITIALIZATION_TASK_RETVAL int
#define INITIALIZATION_TASK_ARG void
#define INITIALIZATION_TASK_NAME main

#define TASK_DEFAULT_RETURN
#define MAIN_DEFAULT_RETURN 0

#define TIMER_COUNT_TO_NS(x) ( (x) / TIMER_FREQUENCY_GHZ )
#define NO_INIT_MAX_TIME_VALUE 0x6FFFFFFF

#define BITSPERLONG 32
#define TOP2BITS(x) ((x & (3L << (BITSPERLONG-2))) >> (BITSPERLONG-2))

#define DO_WORKLOAD(i) \
	do { \
		unsigned long x = 9; \
		unsigned long a = 0L; \
		unsigned long r = 0L; \
		unsigned long e = 0L; \
		int _workload_i_; \
		for (_workload_i_ = 0; _workload_i_ < BITSPERLONG; _workload_i_++) \
		{ \
			r = (r << 2) + TOP2BITS(x); x <<= 2; \
			a <<= 1; \
			e = (a << 1) + 1; \
			if (r >= e) \
			{ \
				r -= e; \
				a++; \
			} \
		} \
		_workload_results[i] = a; \
	} while (0);

#endif
