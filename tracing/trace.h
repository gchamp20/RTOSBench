/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef _NO_TRACE_H_
#define _NO_TRACE_H_

#define NO_BUFFER_SIZE 1310720

typedef struct no_tracing_status_ {
	int on;
	long idx;
} no_tracing_status_t;

extern char no_trace_buffer[];
extern no_tracing_status_t no_tracing_status;

void no_tracing_switch_in(int tid);
void no_tracing_switch_out(int tid);
void no_tracing_switch_ready(int tid);
void no_tracing_inthandler_entry(int irqn);
void no_tracing_inthandler_exit(int irqn);
void no_tracing_write_event(int ev_id);

/**
 * Test IDs definitions
 */

#define NOBENCH_TEST_ID_INT_PROC 0
#define NOBENCH_TEST_ID_COOP_SCHED 2
#define NOBENCH_TEST_ID_SEM_INC 4
#define NOBENCH_TEST_ID_SEM_DEC 6
#define NOBENCH_TEST_ID_SEM_SIGNAL 8
#define NOBENCH_TEST_ID_SEM_BLOCK 10
#define NOBENCH_TEST_ID_SEM_SIGNAL_PRIO 12
#define NOBENCH_TEST_ID_SEM_SIGNAL_WORK 14
#define NOBENCH_TEST_ID_MUTEX_ACQUI 16
#define NOBENCH_TEST_ID_MUTEX_RELEASE 18
#define NOBENCH_TEST_ID_MUTEX_WAKEUP 20
#define NOBENCH_TEST_ID_MUTEX_BLOCK 22
#define NOBENCH_TEST_ID_MUTEX_PIP 24
#define NOBENCH_TEST_ID_MUTEX_WAKEUP_WORK 26
#define NOBENCH_TEST_ID_MQ_SEND 28
#define NOBENCH_TEST_ID_MQ_RECEIVE 30
#define NOBENCH_TEST_ID_MQ_SIGNAL 32
#define NOBENCH_TEST_ID_MQ_BLOCK 34
#define NOBENCH_TEST_ID_MQ_SIGNAL_WORK 36
#define NOBENCH_TEST_ID_PART_JITTER 38

#endif
