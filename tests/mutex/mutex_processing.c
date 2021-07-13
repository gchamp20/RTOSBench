/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_MUTEX_ACQUI
#endif

#ifndef NB_ITER
#define NB_ITER 1000000
#endif

no_task_retval_t mutex_processing_initialize_tests(no_task_argument_t args);
no_task_retval_t task(no_task_argument_t args);

no_task_handle_t task_handle;
no_mutex_t mutex;

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(mutex_processing_initialize_tests);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t mutex_processing_initialize_tests(no_task_argument_t args)
{
	/* Create resources */
	no_mutex_create(&mutex);

	/* Create task */
	task_handle = no_create_task(task,
			"S",
			NO_DECREASE_TASK_PRIO(BASE_PRIO, 1) /* `task` is the only task */
		);

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t task(no_task_argument_t args)
{
	int32_t i;
	DECLARE_TIME_COUNTERS(no_time_t, acqui);
	DECLARE_TIME_COUNTERS(no_time_t, recv);
	DECLARE_TIME_STATS(int64_t);

	/* 1 - Measure mutex acquisition time */
	for (i = 0; i < NB_ITER; i++)
	{
		WRITE_T1_COUNTER(acqui);
		no_mutex_acquire(&mutex);
		WRITE_T2_COUNTER(acqui);
		no_mutex_release(&mutex);
		COMPUTE_TIME_STATS(acqui, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("--- mutex acquisition ---");
	RESET_TIME_STATS();

	/* 2 - Measure mutex release time */
	for (i = 0; i < NB_ITER; i++)
	{
		no_mutex_acquire(&mutex);
		WRITE_T1_COUNTER(recv);
		no_mutex_release(&mutex);
		WRITE_T2_COUNTER(recv);
		COMPUTE_TIME_STATS(recv, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("--- mutex release ---");

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
