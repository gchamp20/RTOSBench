/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_SEM_SIGNAL
#endif

#ifndef NB_ITER
#define NB_ITER 5000
#endif

no_task_retval_t sem_initialize_test(no_task_argument_t args);
no_task_retval_t sender(no_task_argument_t args);
no_task_retval_t receiver(no_task_argument_t args);

no_task_handle_t tasks_handle[2];
no_sem_t sem;

DECLARE_TIME_COUNTERS(no_time_t, s_to_r)
DECLARE_TIME_COUNTERS(no_time_t, r_to_s)

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(sem_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t sem_initialize_test(no_task_argument_t args)
{
	no_sem_create(&sem, 0);

	tasks_handle[0] = no_create_task(sender,
			"S",
			NO_DECREASE_TASK_PRIO(BASE_PRIO, 1) /* sender is the low priority task. */
		);

	tasks_handle[1] = no_create_task(receiver,
			"R",
			BASE_PRIO /* receiver is the high priority task. */
		);

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t sender(no_task_argument_t args)
{
	int32_t i;

	/* 2b - Benchmark. */
	for (i = 0; i < NB_ITER + 1; i++)
	{
		no_sem_signal(&sem);
		WRITE_T2_COUNTER(r_to_s)
	}

	for (i = 0; i < NB_ITER; i++)
	{
		WRITE_T1_COUNTER(s_to_r)
		no_sem_signal(&sem);
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t receiver(no_task_argument_t args)
{
	int32_t i;
	DECLARE_TIME_STATS(int64_t)

	/* 1 - Let sender start */
	no_sem_wait(&sem);

	/* 2a - Benchmark. */
	for (i = 0; i < NB_ITER; i++)
	{
		WRITE_T1_COUNTER(r_to_s)
		no_sem_wait(&sem);
		COMPUTE_TIME_STATS(r_to_s, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("-- Sem: Wait block --")
	RESET_TIME_STATS()

	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&sem);
		WRITE_T2_COUNTER(s_to_r)
		COMPUTE_TIME_STATS(s_to_r, i)
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("-- Sem: Signal unblock --")

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
