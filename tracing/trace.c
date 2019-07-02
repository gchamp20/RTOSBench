/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "trace.h"
#include "porting_layer.h"

#include <stdint.h>

#define NO_EV_ID_SWITCH_IN 0x200
#define NO_EV_ID_SWITCH_OUT 0x201
#define NO_EV_ID_SWITCH_READY 0x202
#define NO_EV_ID_INT_ENTRY 0x203
#define NO_EV_ID_INT_EXIT 0x204

char no_trace_buffer[NO_BUFFER_SIZE];
no_tracing_status_t no_tracing_status;

#pragma pack(push, 1)
typedef struct no_trace_event_1 {
	int32_t id;
	int32_t value;
	int64_t timestamp;
} no_trace_event_1_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct no_trace_event {
	int32_t id;
	int64_t timestamp;
} no_trace_event_t;
#pragma pack(pop)

typedef struct no_trace_record {
	long idx;
	int64_t timestamp;
} no_trace_record_t;

static no_trace_record_t reserve_memory_buffer(int size);
static int CAS(long* dest, long new_value, long old_value);

static int CAS(long* dest, long new_value, long old_value)
{
	int store_result;
	do
	{
		int val;
		asm volatile ("ldrex %0, [%1]" : "=r" (val) : "r" (dest));
		if(val != old_value)
		{
			return 1;
		}
		asm volatile ("strex %0, %1, [%2]" : "=r" (store_result) : "r" (new_value), "r" (dest));
	} while (store_result != 0);
	return 0;
}

static no_trace_record_t reserve_memory_buffer(int size)
{
	long old_idx;
	long cur_idx;
	no_trace_record_t record;
	do
	{
		old_idx = no_tracing_status.idx;
		record.timestamp = (int64_t)no_time_get();
		cur_idx = no_tracing_status.idx + size;
		record.idx = old_idx;
		if (old_idx > NO_BUFFER_SIZE) {
			break;
		}
	} while (CAS(&no_tracing_status.idx, cur_idx, old_idx) != 0);
	return record;
}

void no_tracing_switch_in(int tid)
{
	if (no_tracing_status.on) {
		no_trace_event_1_t* ev;
		no_trace_record_t record = reserve_memory_buffer(sizeof(no_trace_event_1_t));
		if (record.idx < NO_BUFFER_SIZE) {
			ev = (no_trace_event_1_t*)&no_trace_buffer[record.idx];
			ev->id = NO_EV_ID_SWITCH_IN;
			ev->timestamp = record.timestamp;
			ev->value = tid;
		}
	}
}

void no_tracing_switch_out(int tid)
{
	if (no_tracing_status.on) {
		no_trace_event_1_t* ev;
		no_trace_record_t record = reserve_memory_buffer(sizeof(no_trace_event_1_t));
		if (record.idx < NO_BUFFER_SIZE) {
			ev = (no_trace_event_1_t*)&no_trace_buffer[record.idx];
			ev->id = NO_EV_ID_SWITCH_OUT;
			ev->timestamp = record.timestamp;
			ev->value = tid;
		}
	}
}

void no_tracing_switch_ready(int tid)
{
	if (no_tracing_status.on) {
		no_trace_event_1_t* ev;
		no_trace_record_t record = reserve_memory_buffer(sizeof(no_trace_event_1_t));
		if (record.idx < NO_BUFFER_SIZE) {
			ev = (no_trace_event_1_t*)&no_trace_buffer[record.idx];
			ev->id = NO_EV_ID_SWITCH_READY;
			ev->timestamp = record.timestamp;
			ev->value = tid;
		}
	}
}

void no_tracing_write_event(int ev_id)
{
	if (no_tracing_status.on) {
		no_trace_event_t* ev;
		no_trace_record_t record = reserve_memory_buffer(sizeof(no_trace_event_t));
		if (record.idx < NO_BUFFER_SIZE) {
			ev = (no_trace_event_t*)&no_trace_buffer[record.idx];
			ev->id = ev_id;
			ev->timestamp = record.timestamp;
		}
	}
}

void no_tracing_interrupt_entry(int irqn)
{
	if (no_tracing_status.on) {
		no_trace_event_t* ev;
		no_trace_record_t record = reserve_memory_buffer(sizeof(no_trace_event_t));
		if (record.idx < NO_BUFFER_SIZE) {
			ev = (no_trace_event_t*)&no_trace_buffer[record.idx];
			ev->id = NO_EV_ID_INT_ENTRY;
			ev->timestamp = record.timestamp;
		}
	}
}

void no_tracing_interrupt_exit(int irqn)
{
	if (no_tracing_status.on) {
		no_trace_event_t* ev;
		no_trace_record_t record = reserve_memory_buffer(sizeof(no_trace_event_t));
		if (record.idx < NO_BUFFER_SIZE) {
			ev = (no_trace_event_t*)&no_trace_buffer[record.idx];
			ev->id = NO_EV_ID_INT_EXIT;
			ev->timestamp = record.timestamp;
		}
	}
}
