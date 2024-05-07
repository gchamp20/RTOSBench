/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_SEM_SIGNAL_PRIO
#endif

#ifndef NB_ITER
#define NB_ITER 5000
#endif

#ifndef NB_TASK
#define NB_TASK 1
#endif

no_task_retval_t sem_prio_initialize_test(no_task_argument_t args);
no_task_retval_t TA_high_prio(no_task_argument_t args);
no_task_retval_t TB_med_prio(no_task_argument_t args);
no_task_retval_t TC_low_prio(no_task_argument_t args);

no_task_handle_t tasks_handle[2];

no_task_handle_t med_tasks_handle[NB_TASK];
char workload_tasks_name[NB_TASK][4];

no_sem_t aux_sem;
no_sem_t sem;

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(sem_prio_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t sem_prio_initialize_test(no_task_argument_t args)
{
	int i;

	no_sem_create(&sem, 0);
	no_sem_create(&aux_sem, 0);

	tasks_handle[0] = no_create_task(TC_low_prio,
			"L",
			NO_DECREASE_TASK_PRIO(BASE_PRIO, 2) /* TC_low_prio is the low priority task. */
		);

	for (i = 0; i < NB_TASK; i++)
	{
		workload_tasks_name[i][0] = 65;
		workload_tasks_name[i][1] = (65 + i) % 255;
		workload_tasks_name[i][2] = 0;
		workload_tasks_name[i][3] = 0;

		med_tasks_handle[i] = no_create_task(TB_med_prio,
				workload_tasks_name[i],
				BASE_PRIO - 1
			);
	}

	tasks_handle[1] = no_create_task(TA_high_prio,
			"H",
			BASE_PRIO /* TA_high_prio is the high priority task. */
		);

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t TC_low_prio(no_task_argument_t args)
{
	int i;

	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_signal(&aux_sem);
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t TB_med_prio(no_task_argument_t args)
{
	int i;

	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&sem);
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t TA_high_prio(no_task_argument_t args)
{
	int i;
	DECLARE_TIME_COUNTERS(no_time_t, time);
	DECLARE_TIME_STATS(int64_t);

	/*  Measure signaling time with priority:
	 *
	 * 1 - TA blocks waiting on aux_sem
	 * 2 - TB blocks waiting on sem
	 * 3 - TC signals aux_sem
	 * 4a - TA reads t1
	 * 4b - TA signals aux
	 * 		- OS moves TB to ready
	 * 4c - TA reads t2
	 * 5 - Go back to 1
	 */
	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&aux_sem);

		WRITE_T1_COUNTER(time);
		no_sem_signal(&sem);
		WRITE_T2_COUNTER(time);

		COMPUTE_TIME_STATS(time, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("--- sem singaling with prio ---");

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
