/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_SEM_INC
#endif

#ifndef NB_ITER
#define NB_ITER 5000
#endif

no_task_retval_t sem_processing_initialize_test(no_task_argument_t args);
no_task_retval_t task(no_task_argument_t args);

no_task_handle_t task_handle;
no_sem_t sem;

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(sem_processing_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t sem_processing_initialize_test(no_task_argument_t args)
{
	no_sem_create(&sem, 0);

	task_handle = no_create_task(task,
			"S",
			NO_DECREASE_TASK_PRIO(BASE_PRIO, 1) /* `task` is the only task */
		);

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t task(no_task_argument_t args)
{
	int32_t i;
	DECLARE_TIME_COUNTERS(no_time_t, inc);
	DECLARE_TIME_COUNTERS(no_time_t, dec);
	DECLARE_TIME_STATS(int64_t);
	unsigned long _workload_results[NB_ITER];

	/* 1b - Measure semaphore signaling time */
	for (i = 0; i < NB_ITER; i++)
	{
		WRITE_T1_COUNTER(inc);
		no_sem_signal(&sem);
		WRITE_T2_COUNTER(inc);
		no_sem_wait(&sem);
		COMPUTE_TIME_STATS(inc, i);
		DO_WORKLOAD(i)
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("--- Sem: Signal ---");

	RESET_TIME_STATS();

	/* 2b - Measure semaphore acquisition time */
	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_signal(&sem);
		WRITE_T1_COUNTER(dec);
		no_sem_wait(&sem);
		WRITE_T2_COUNTER(dec);
		COMPUTE_TIME_STATS(dec, i);
		DO_WORKLOAD(i)
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("--- Sem: Wait ---");

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
