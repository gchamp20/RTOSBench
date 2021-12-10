/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_MUTEX_PIP
#endif

#ifndef NB_ITER
#define NB_ITER 1000000
#endif

no_task_retval_t mutex_processing_initialize_tests(no_task_argument_t args);

no_task_retval_t TA_high_prio(no_task_argument_t args);
no_task_retval_t TB_med_prio(no_task_argument_t args);
no_task_retval_t TC_low_prio(no_task_argument_t args);

no_task_handle_t tasks_handle[3];

no_mutex_t mutex;
no_sem_t aux_sem;
no_sem_t aux_sem2;

DECLARE_TIME_COUNTERS(no_time_t, time);

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(mutex_processing_initialize_tests);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t mutex_processing_initialize_tests(no_task_argument_t args)
{
	/* Create resources */
	no_mutex_create(&mutex);
	no_sem_create(&aux_sem, 0);
	no_sem_create(&aux_sem2, 0);

	/* Create task */
	tasks_handle[0] = no_create_task(TC_low_prio,
			"L",
			NO_DECREASE_TASK_PRIO(BASE_PRIO, 2) /* TC_low_prio is the low priority task. */
		);

	tasks_handle[1] = no_create_task(TB_med_prio,
			"M",
			NO_DECREASE_TASK_PRIO(BASE_PRIO, 1) /* TB_med_prio is the med priority task. */
		);

	tasks_handle[2] = no_create_task(TA_high_prio,
			"H",
			BASE_PRIO /* TA_high_prio is the high priority task. */
		);

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t TC_low_prio(no_task_argument_t args)
{
	int i;
	DECLARE_TIME_STATS(int64_t);

	for (i = 0; i < NB_ITER; i++)
	{
		no_mutex_acquire(&mutex);
		no_sem_signal(&aux_sem);
		WRITE_T2_COUNTER(time);
		COMPUTE_TIME_STATS(time, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
		no_mutex_release(&mutex);
	}

	REPORT_BENCHMARK_RESULTS("--- Mutex PIP---");

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t TB_med_prio(no_task_argument_t args)
{
	int i;

	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&aux_sem2);
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t TA_high_prio(no_task_argument_t args)
{
	int i;

	/*  Measure signaling time with priority:
	 *
	 * 1 - TA blocks waiting on aux_sem
	 * 2 - TB blocks waiting on aux_sem2
	 * 3 - TC acquires mutex
	 * 4 - TC signals aux_sem
	 * 5 - TA signals aux_sem2
	 * 6a - TA reads time
	 * 6 - TA tries to acquire mutex
	 * 7 - TC reads time (PIP)
	 * 8 - TC release mutex
	 */
	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&aux_sem);
		no_sem_signal(&aux_sem2);
		WRITE_T1_COUNTER(time);
		no_mutex_acquire(&mutex);
		no_mutex_release(&mutex);
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
