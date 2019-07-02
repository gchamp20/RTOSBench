/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <time.h>

/**
 * Parameters
 */

#ifndef BASE_PRIO
#define BASE_PRIO 10
#endif

/**
 * Type definitions
 */
typedef void* no_task_retval_t;
typedef void* no_task_argument_t;
typedef int no_main_retval_t;
typedef int no_main_argument_t;
typedef no_task_retval_t (*no_task_entry_t)(no_task_argument_t);
typedef pthread_t no_task_handle_t;
typedef struct timespec no_time_t;
typedef sem_t no_sem_t;
typedef pthread_mutex_t no_mutex_t;
typedef pthread_cond_t no_event_t;
typedef mqd_t no_mq_t;
typedef void (*no_int_handler_t)(int, void*);

/**
 * Macros
 */
#define INITIALIZATION_TASK_RETVAL int
#define INITIALIZATION_TASK_ARG void
#define INITIALIZATION_TASK_NAME main

#define TASK_DEFAULT_RETURN NULL
#define MAIN_DEFAULT_RETURN 0

#define NO_INIT_MAX_TIME_VALUE 0x7FFFFFFF

#ifndef TRACING
#ifndef NO_VERBOSE_RESULTS
#define REPORT_BENCHMARK_RESULTS(STR_PTR) do { \
			no_serial_write(STR_PTR); \
			no_result_report(max_cycles, min_cycles, average_cycles); \
			fflush(stdout); \
		} while (0);
#else
#define REPORT_BENCHMARK_RESULTS(STR_PTR) do { \
			for (int _i; _i < NB_ITER; _i++) { \
				no_single_result_report("", no_cycles_results[_i]); \
			} \
		} while (0);
#endif
#endif

#define BITSPERLONG 32
#define TOP2BITS(x) ((x & (3L << (BITSPERLONG-2))) >> (BITSPERLONG-2))

#define DO_WORKLOAD(i) \

#endif
