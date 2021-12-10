/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"
#include <math.h>

#ifdef TRACING
#define NO_TEST_ID 10
#endif

#ifndef NB_ITER
#define NB_ITER 1000
#endif

#define NB_TASK 0

no_task_retval_t monitor(no_task_argument_t args);
no_task_retval_t task(no_task_argument_t args);
no_task_retval_t jitter_initialize_test(no_task_argument_t args);

no_task_handle_t tasks_handle[NB_TASK + 1];
char tasks_name[NB_TASK][5];

DECLARE_TIME_COUNTERS(no_time_t, _)

no_main_retval_t main(no_main_argument_t args)
{
	no_initialize_test(jitter_initialize_test);
	return MAIN_DEFAULT_RETURN;
}

no_task_retval_t jitter_initialize_test(no_task_argument_t args)
{
	int32_t i;

	for (i = 0 ; i < NB_TASK; i++)
	{
		tasks_name[i][0] = 65;
		tasks_name[i][1] = (65 + i) % 255;
		tasks_name[i][2] = (66 + i) % 255;
		tasks_name[i][3] = (67 + i) % 255;
		tasks_name[i][4] = '\0';
		tasks_handle[i] = no_create_task(task, tasks_name[i], NO_DECREASE_TASK_PRIO(BASE_PRIO, 1));
	}

	tasks_handle[NB_TASK + 1] = no_create_task(monitor, "MON", BASE_PRIO);
}

no_task_retval_t task(no_task_argument_t args)
{
	int32_t i;

	for (i = 0; i < NB_ITER; i++)
	{
		no_task_yield();
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}

no_task_retval_t monitor(no_task_argument_t args)
{
	int32_t i;
	long diff;

	no_time_t time_t1;
	no_time_t time_t2;
	DECLARE_TIME_STATS(int64_t);

	for (i = 0; i < NB_ITER; i++)
	{
		time_t1 = no_time_get();
		time_t1 = no_add_times(&time_t1, 500000000);

		no_task_delay(400);

		time_t2 = no_time_get();
		diff = no_time_diff(&time_t1, &time_t2);
		no_single_result_report("", diff);

		no_task_yield();
	}

	no_task_suspend_self();

	return TASK_DEFAULT_RETURN;
}
