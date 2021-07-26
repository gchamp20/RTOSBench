/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef PORTING_LAYER_H_
#define PORTING_LAYER_H_

#include "config.h"

#ifndef NO_PART_PERIOD_NS
	#define NO_PART_PERIOD_NS 100000000
#endif

#ifndef NO_PART_RUNTIME_NS
	#define NO_PART_RUNTIME_NS 100000000
#endif

//#define NO_PART_MAX_GAP NO_PART_PERIOD_NS - NO_PART_RUNTIME_NS
#define NO_PART_MAX_GAP 100000000

#ifndef NO_INIT_MIN_TIME_VALUE
	#define NO_INIT_MIN_TIME_VALUE 0
#endif

#ifndef NO_INIT_MAX_TIME_VALUE
	#define NO_INIT_MAX_TIME_VALUE 0x7FFFFFFFFFFFFFFF
#endif

#ifndef NO_INIT_TIME_STATS_AVERAGE
	#define NO_INIT_TIME_STATS_AVERAGE(avg, cyc, n) ((long)avg + (((long)cyc - (long)avg) / (n + 1)))
#endif

//#define LOG_DEBUG(x) no_serial_write((x));
#define LOG_DEBUG(x)

#ifndef TRACING
// NORMAL MODE

#ifndef DO_WORKLOAD
#define BITSPERLONG 64
#define TOP2BITS(x) ((x & (3L << (BITSPERLONG-2))) >> (BITSPERLONG-2))

#define DO_WORKLOAD(i) \
	do { \
		unsigned long x = 9; \
		unsigned long a = 0L; \
		unsigned long r = 0L; \
		unsigned long e = 0L; \
		int _workload_i_; \
		for (_workload_i_ = 0; _workload_i_ < BITSPERLONG; _workload_i_++) \
		{ \
			r = (r << 2) + TOP2BITS(x); x <<= 2; \
			a <<= 1; \
			e = (a << 1) + 1; \
			if (r >= e) \
			{ \
				r -= e; \
				a++; \
			} \
		} \
		_workload_results[i] = a; \
	} while (0);
#endif // DO_WORKLOAD

#define DECLARE_TIME_COUNTERS(TYPE, SUFFIX) \
	TYPE SUFFIX##_t1; \
	TYPE SUFFIX##_t2;

#ifndef NO_VERBOSE_RESULTS

#define DECLARE_TIME_STATS(TYPE) \
	TYPE cycles; \
	TYPE max_cycles = NO_INIT_MIN_TIME_VALUE; \
	TYPE min_cycles = NO_INIT_MAX_TIME_VALUE; \
	TYPE average_cycles = NO_INIT_MIN_TIME_VALUE; \
	unsigned num_measurements = 0; \

#else

#define DECLARE_TIME_STATS(TYPE) \
	TYPE cycles; \
	TYPE no_cycles_results[NB_ITER]; \

#endif // NO_VERBOSE_RESULTS

#define WRITE_T1_COUNTER(SUFFIX) \
	SUFFIX##_t1 = no_time_get();

#define WRITE_T2_COUNTER(SUFFIX) \
	SUFFIX##_t2 = no_time_get();

#ifndef NO_VERBOSE_RESULTS
#define COMPUTE_TIME_STATS(SUFFIX, i) do { \
			cycles = no_time_diff(&SUFFIX##_t1, &SUFFIX##_t2); \
			if (cycles < NO_PART_MAX_GAP) { \
				if (cycles > max_cycles) \
				{ \
					max_cycles = cycles; \
				} \
				if (cycles <  min_cycles) \
				{ \
					min_cycles = cycles; \
				} \
				average_cycles += (cycles - average_cycles) / (num_measurements + 1); \
				num_measurements++; \
			}\
		} while(0);
#else
#define COMPUTE_TIME_STATS(SUFFIX, i) do { \
			cycles = no_time_diff(&SUFFIX##_t1, &SUFFIX##_t2); \
			no_cycles_results[i] = cycles; \
		} while(0);
#endif // NO_VERBOSE_RESULTS

#ifndef NO_VERBOSE_RESULTS
#define RESET_TIME_STATS() \
	average_cycles = 0; \
	max_cycles = 0; \
	min_cycles = NO_INIT_MAX_TIME_VALUE; \
	num_measurements = 0; \

#else
#define RESET_TIME_STATS()
#endif // NO_VERBOSE_RESULTS

#ifndef REPORT_BENCHMARK_RESULTS
#ifndef NO_VERBOSE_RESULTS
#define REPORT_BENCHMARK_RESULTS(STR_PTR) do { \
			no_serial_write(STR_PTR); \
			no_result_report(max_cycles, min_cycles, average_cycles); \
		} while (0);
#else
#define REPORT_BENCHMARK_RESULTS(STR_PTR) do { \
			for (int _i; _i < NB_ITER; _i++) { \
				no_single_result_report("", no_cycles_results[_i]); \
			} \
		} while (0);
#endif // NO_VERBOSE_RESULTS
#endif // REPORT_BENCHMARK_RESULTS

#else
// TRACING MODE
#include "trace.h"
#define DECLARE_TIME_COUNTERS(TYPE, SUFFIX) \
	int32_t SUFFIX##_ev_id_start = NO_TEST_ID + __COUNTER__; \
	int32_t SUFFIX##_ev_id_end = NO_TEST_ID + __COUNTER__;

#define DECLARE_TIME_STATS(TYPE) \

#define WRITE_T1_COUNTER(SUFFIX) \
	no_tracing_write_event(SUFFIX##_ev_id_start);

#define WRITE_T2_COUNTER(SUFFIX) \
	no_tracing_write_event(SUFFIX##_ev_id_end);

#define COMPUTE_TIME_STATS(SUFFIX, i) \

#define RESET_TIME_STATS() \

#ifndef REPORT_BENCHMARK_RESULTS
#define REPORT_BENCHMARK_RESULTS(STR_PTR) do { \
			no_serial_write("Results are in the trace buffer"); \
			no_tracing_report(); \
		} while (0);
#endif // REPORT_BENCHMARK_RESULTS

#endif // TRACING

#ifndef NO_DECREASE_TASK_PRIO
#define NO_DECREASE_TASK_PRIO(prio, decrease_by) (prio - decrease_by)
#endif // NO_DECREASE_TASK_PRIO

/**
 * Porting API
 */

/*
 * @brief Call to initialize the a test case.
 */
void no_initialize_test(no_task_entry_t init_test_fct);

/*
 * @brief Creates a task. The task must be in a suspended or dormant state after creation.
 */
no_task_handle_t no_create_task(no_task_entry_t task_entry, char task_name[4], unsigned int prio);

/**
 * @brief Changes the state of the running task to ready.
 */
void no_task_yield();

/**
 * @brief Suspends the calling task
 */
void no_task_suspend_self();

/**
 * @brief Delays the calling task by the specified number of nanoseconds
 */
void no_task_delay(unsigned int nanoseconds);

/**
 * @brief Reads the current value of a time.
 */
no_time_t no_time_get();

/**
 * @brief Prints the results on the serial output
 */
void no_result_report(int64_t max, int64_t min, int64_t average);

/**
 * @brief Write a single integer value with a prefix
 */
void no_single_result_report(char* prefix, int64_t time);

/**
 * @brief Computes the addition of a time value and a number of tick
 */
no_time_t no_add_times(const no_time_t* base, unsigned int ticks);

/**
 * @brief Computes the difference between two time values (t2 -t1)
 */
long no_time_diff(const no_time_t* t1, const no_time_t* t2);

/**
 * @brief Creates a semaphore.
 */
void no_sem_create(no_sem_t* sem, int current_value);

/**
 * @brief Wait on a semaphore.
 */
void no_sem_wait(no_sem_t* sem);

/**
 * @brief Signal a semaphore.
 */
void no_sem_signal(no_sem_t* sem);

/**
 * @brief Creates a mutex.
 */
void no_mutex_create(no_mutex_t* mutex);

/**
 * @brief Acquire a mutex .
 */
void no_mutex_acquire(no_mutex_t* mutex);

/**
 * @brief Release a mutex.
 */
void no_mutex_release(no_mutex_t* mutex);

/**
 * @brief Creates an event.
 */
void no_event_create(no_event_t* event);

/**
 * @brief Set an event.
 */
void no_event_set(no_event_t* event);

/**
 * @brief Wait on an event.
 */
void no_event_wait(no_event_t* event);

/**
 * @brief Reset an event.
 */
void no_event_reset(no_event_t* event);

/**
* @brief Creates a message queue.
*/
void no_mq_create(no_mq_t* mq, unsigned int length, unsigned int msgsize);

/**
* @brief Send a message to a message queue.
*/
void no_mq_send(no_mq_t* mq, unsigned int msg);

/**
* @brief Receive a message from a message queue.
*/
void no_mq_receive(no_mq_t* mq);

/**
 * @brief Writes a string on the serial output
 */
void no_serial_write(const char* string);

/**
 * @brief Create a software generated interurpt
 */
void no_interrupt_do_sgi();

/**
 * @brief Enable sgi.
 */
void no_interrupt_enable_sgi();

/**
 * @brief Register sgi handler.
 */
void no_interrupt_register_sgi_handler(no_int_handler_t fn);

#ifndef NO_DECLARE_INT_HANDLER
#define NO_DECLARE_INT_HANDLER(NAME, TIME_CNTR_SUFFIX) \
void NAME(int nIRQ, void *pParam) { \
	WRITE_T2_COUNTER(TIME_CNTR_SUFFIX) \
}
#endif

void no_cycle_reset_counter();

#ifdef TRACING
void no_tracing_report();
#endif

#endif
