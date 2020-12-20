/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#include "math.h"

#ifdef TRACING
#define NO_TEST_ID NOBENCH_TEST_ID_COOP_SCHED
#endif

#ifndef NB_ITER
#define NB_ITER 1000
#endif

#define NB_TASK 5

no_task_retval_t task(no_task_argument_t args);
no_task_retval_t round_robin_stress_initialize_test(no_task_argument_t args);

no_task_handle_t tasks_handle[NB_TASK];
char tasks_name[NB_TASK][5];

DECLARE_TIME_COUNTERS(no_time_t, _)

volatile int tasks_idx;
volatile int tasks_done_count;

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(round_robin_stress_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t round_robin_stress_initialize_test(no_task_argument_t args)
{
	int32_t i;

	tasks_idx = 0;
	tasks_done_count = 0;

	for (i = 0 ; i < NB_TASK; i++)
	{
		tasks_name[i][0] = 65;
		tasks_name[i][1] = (65 + i) % 255;
		tasks_name[i][2] = (66 + i) % 255;
		tasks_name[i][3] = (67 + i) % 255;
		tasks_name[i][4] = '\0';
		tasks_handle[i] = no_create_task(task, tasks_name[i], BASE_PRIO);
	}
}

no_task_retval_t task(no_task_argument_t args)
{
	int32_t i;
	int32_t local_idx;

	local_idx = tasks_idx++;
	if (local_idx == 0)
	{
		DECLARE_TIME_STATS(int64_t)
		for (i = 0; i < NB_ITER; i++)
		{
			//DO_WORKLOAD(i)
			WRITE_T1_COUNTER(_)
			no_task_yield();
			if (i > 0)
			{
				COMPUTE_TIME_STATS(_, i);
			}
		}
		REPORT_BENCHMARK_RESULTS("-- round robin context switch results --")
	}
	else if (local_idx == 1)
	{
		for (i = 0; i < NB_ITER; i++)
		{
			//DO_WORKLOAD(i)
			no_task_yield();
			WRITE_T2_COUNTER(_)
		}
	}
	else
	{
		for (i = 0; i < NB_ITER; i++)
		{
			//DO_WORKLOAD(i)
			no_task_yield();
		}
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
