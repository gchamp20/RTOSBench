/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_MUTEX_WAKEUP
#endif

#ifndef NB_ITER
#define NB_ITER 1000000
#endif

no_task_retval_t mutex_initialize_test(no_task_argument_t args);
no_task_retval_t sender(no_task_argument_t args);
no_task_retval_t receiver(no_task_argument_t args);

no_task_handle_t tasks_handle[2];
no_sem_t aux_sem;
no_mutex_t mutex;

DECLARE_TIME_COUNTERS(no_time_t, s_to_r)
DECLARE_TIME_COUNTERS(no_time_t, r_to_s)

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(mutex_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t mutex_initialize_test(no_task_argument_t args)
{
	no_sem_create(&aux_sem, 0);
	no_mutex_create(&mutex);

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

	/* 1b - Benchmark. */
	for (i = 0; i < NB_ITER * 2; i++)
	{
		no_mutex_acquire(&mutex);
		no_sem_signal(&aux_sem);
		WRITE_T2_COUNTER(r_to_s)
		WRITE_T1_COUNTER(s_to_r)
		no_mutex_release(&mutex);
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t receiver(no_task_argument_t args)
{
	int32_t i;
	DECLARE_TIME_STATS(int64_t)

	/* 1a - Benchmark. */
	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&aux_sem);
		no_mutex_acquire(&mutex);
		WRITE_T2_COUNTER(s_to_r)
		no_mutex_release(&mutex);
		COMPUTE_TIME_STATS(s_to_r, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("-- Mutex release unblock --")
	RESET_TIME_STATS()

	for (i = 0; i < NB_ITER; i++)
	{
		no_sem_wait(&aux_sem);
		WRITE_T1_COUNTER(r_to_s)
		no_mutex_acquire(&mutex);
		no_mutex_release(&mutex);
		COMPUTE_TIME_STATS(r_to_s, i)
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("-- Mutex request block --")

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
