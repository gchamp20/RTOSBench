/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_MQ_SEND
#endif

#ifndef NB_ITER
#define NB_ITER 1000000
#endif

#define QUEUE_DEPTH 50

no_task_retval_t mq_initialize_test(no_task_argument_t args);
no_task_retval_t task(no_task_argument_t args);

no_task_handle_t task_handle;
no_mq_t mq;

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(mq_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t mq_initialize_test(no_task_argument_t args)
{
	/* Create resources */
	no_mq_create(&mq, QUEUE_DEPTH, sizeof(int32_t));

	task_handle = no_create_task(task,
			"S",
			BASE_PRIO /* `task` is the low priority task. */
		);

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t task(no_task_argument_t args)
{
	int32_t i;
	DECLARE_TIME_COUNTERS(no_time_t, send)
	DECLARE_TIME_COUNTERS(no_time_t, recv)
	DECLARE_TIME_STATS(int64_t)

	/* 1a - Measure MQ send . */
	for (i = 0; i < NB_ITER; i++)
	{
		WRITE_T1_COUNTER(send)
		no_mq_send(&mq, 1);
		WRITE_T2_COUNTER(send)
		no_mq_receive(&mq);
		COMPUTE_TIME_STATS(send, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("--- MQ Send ---");
	RESET_TIME_STATS();

	/* 1a - Measure MQ receive. */
	for (i = 0; i < NB_ITER; i++)
	{
		no_mq_send(&mq, 1);
		WRITE_T1_COUNTER(recv)
		no_mq_receive(&mq);
		WRITE_T2_COUNTER(recv)
		COMPUTE_TIME_STATS(recv, i);
#ifndef TRACING
		no_cycle_reset_counter();
#endif
	}

	REPORT_BENCHMARK_RESULTS("-- MQ Receive --")

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
