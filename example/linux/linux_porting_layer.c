/* Copyright (c) 2019, Guillaume Champagne
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "porting_layer.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#ifdef TRACING
#include "tracing/tp.h"
#endif

#define BASE_THREAD_COUNT 10

#ifndef NO_AFFINITY
#define NO_AFFINITY 7
#endif

#define USEC_PER_SEC		1000000
#define NSEC_PER_SEC		1000000000

pthread_t* thread_pool;
unsigned int thread_count;
unsigned int thread_count_limit;

pthread_mutex_t event_condvar_mutex;

static void set_latency_target(void);
static inline void tsnorm(struct timespec *ts);

void no_initialize_test(no_task_entry_t init_function)
{
	struct sched_param sched_param;
	cpu_set_t cpuset;

	/* seed */
	srand(time(0));

	/* Allocate thread pool */
	thread_pool = malloc(BASE_THREAD_COUNT * sizeof(pthread_t));
	thread_count = 0;
	thread_count_limit = BASE_THREAD_COUNT;

	/* Allocate sem pool */
	CPU_ZERO(&cpuset);
	CPU_SET(NO_AFFINITY, &cpuset);
	printf("setting up affinity %d\n", NO_AFFINITY);

	if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) {
		no_serial_write("Sched set affinity failed.\n");
		return;
	}

	char* prio_env = getenv("NO_PRIO");
	int prio = BASE_PRIO;
	if (prio_env != NULL) {
		prio = atoi(prio_env);
	}

	printf("setting up prio %d\n", prio);
	sched_param.sched_priority = prio;
	if (sched_setscheduler(0, SCHED_FIFO, &sched_param) != 0) {
		no_serial_write("Sched set scheduler failed.\n");
		return;
	}

	set_latency_target();

	/* Init test */
	init_function(NULL);

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigwait(&set, NULL);
}

no_task_handle_t no_create_task(no_task_entry_t task_entry, char task_name[4], unsigned int prio)
{
	pthread_attr_t attr;
	struct sched_param sched_param;

	if (thread_count >= thread_count_limit) {
		/* Double the size of the thread pool and the sem pool */
		pthread_t* new_thread_pool = malloc(thread_count_limit * 2 * sizeof(pthread_t));

		memcpy((void*)new_thread_pool, (void*)thread_pool, thread_count_limit * sizeof(pthread_t));

		free(thread_pool);

		thread_pool = new_thread_pool;

		thread_count_limit = thread_count_limit * 2;
	}

	/* Set SCHED_FIFO policy and thread priority */
	int err = pthread_attr_init(&attr);
	sched_param.sched_priority = prio;
	if (err != 0) {
		no_serial_write("Attr init failed\n");
	}

	err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if (err != 0) {
		no_serial_write("Set sched policy failed\n");
	}

	err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if (err != 0) {
		no_serial_write("Set inherit sched param failed\n");
	}

	err = pthread_attr_setschedparam(&attr, &sched_param);
	if (err != 0) {
		no_serial_write("Set sched param failed\n");
	}

	err = pthread_create(&thread_pool[thread_count], &attr, task_entry, NULL);
	pthread_attr_destroy(&attr);

	/* Create associated semaphore */

	thread_count++;

	if (err != 0) {
		no_serial_write("Task creation failed\n");
	}

	return (no_task_handle_t)thread_pool[thread_count - 1];
}

void no_task_yield()
{
	sched_yield();
}

void no_task_suspend(no_task_handle_t task)
{
}

void no_task_suspend_self()
{
}

void no_task_resume(no_task_handle_t task)
{
}

void no_task_delay(unsigned int ticks)
{
	struct timespec req;
	long nsec = ticks * 1000000L;
	long sec = (nsec) / 1000000000L;
	nsec = nsec % 1000000000L;
	req.tv_nsec = nsec;
	req.tv_sec = sec;
	nanosleep(&req, NULL);
}

void no_init_timer()
{
}

void no_disable_timer()
{
}

void no_reset_timer()
{
}

no_time_t no_add_times(const no_time_t* base, unsigned int ticks)
{
	struct timespec* ts = (struct timespec*)base;
	struct timespec time;
	time.tv_nsec = ticks + ts->tv_nsec;
	time.tv_sec = ts->tv_sec;
	tsnorm(&time);
	return time;
}

no_time_t no_time_get()
{
	struct timespec res;
	clock_gettime(CLOCK_MONOTONIC, &res);
	return res;
}

/** TAKEN FOR rt-test **/
long no_time_diff(const no_time_t* t1, const no_time_t* t2)
{
	long diff;
	diff = NSEC_PER_SEC * (int64_t)((int) t2->tv_sec - (int) t1->tv_sec);
	//printf("sec diff=%ld\n", diff);
	diff += ((int) t2->tv_nsec - (int) t1->tv_nsec);
	/*printf("nsec t2 - t1; %ld - %ld\n", t2->tv_nsec, t1->tv_nsec);
	printf("sec t2 - t1; %ld - %ld", t2->tv_sec, t1->tv_sec);
	printf("final diff=%ld\n", diff);*/
	return diff;
}

void no_sem_create(no_sem_t* sem, int current_value)
{
	sem_init((sem_t*)sem, 0, current_value);
}

void no_sem_wait(no_sem_t* sem)
{
	sem_wait((sem_t*)sem);
}

void no_sem_signal(no_sem_t* sem)
{
	sem_post((sem_t*)sem);
}

void no_mutex_create(no_mutex_t* mutex)
{
	pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
}

void no_mutex_acquire(no_mutex_t* mutex)
{
	pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void no_mutex_release(no_mutex_t* mutex)
{
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

void no_event_create(no_event_t* event)
{
	pthread_mutex_init(&event_condvar_mutex, NULL);
	pthread_cond_init((pthread_cond_t*)event, NULL);
	pthread_mutex_lock(&event_condvar_mutex);
}

void no_event_set(no_event_t* event)
{
	pthread_cond_broadcast((pthread_cond_t*)event);
}

void no_event_wait(no_event_t* event)
{
	pthread_cond_wait(event, &event_condvar_mutex);
}

void no_event_reset(no_event_t* event)
{
}

static void gen_random_str(char *s, int len) {
	int i;
	static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVXYWZabcdefghijklmnoqrstuvxyz123456789";
	for (i = 0; i < len; i++) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
}

void no_mq_create(no_mq_t* mq, unsigned int length, unsigned int msgsize)
{
	char name[18];
	struct mq_attr attr;
	mqd_t* mq_handle = (mqd_t*)mq;

	attr.mq_maxmsg = length;
	attr.mq_msgsize = msgsize;

	name[0] = '/';
	name[17] = '\0';
	gen_random_str(&name[1], 16);

	*mq_handle = mq_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
	if (*mq_handle == -1) {
		printf("mq create error\n");
	}
}

void no_mq_send(no_mq_t* mq, unsigned int msg)
{
	mq_send(*((mqd_t*)mq), (const char*)&msg, sizeof(msg), 1);
}

void no_mq_receive(no_mq_t* mq)
{
	unsigned int msg;
	int err = mq_receive(*((mqd_t*)mq), (char*)&msg, sizeof(msg), NULL);
	if (err == -1) {
		printf("error mq receive\n");
	}
}

void no_serial_write(const char* string)
{
	printf("%s\n", string);
}

void no_cycle_reset_counter()
{
}

unsigned int no_cycle_get_count()
{
	struct timespec res;
	clock_gettime(CLOCK_MONOTONIC, &res);
	return res.tv_nsec;
}

void no_result_report(int64_t max, int64_t min, int64_t average)
{
	int a, b, c;
	a = (int)max;
	b = (int)min;
	c = (int)average;
	printf("max=%d\nmin=%d\naverage=%d\n", a, b, c);
}

void no_single_result_report(char* prefix, int64_t value)
{
	int a = (int)value;
	printf("%s%d\n", prefix, a);
}

/** TAKEN FOR rt-test **/
static void set_latency_target(void)
{
	struct stat s;
	int err;

	errno = 0;
	err = stat("/dev/cpu_dma_latency", &s);
	if (err == -1) {
		return;
	}

	errno = 0;
	int latency_target_fd;
	latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
	if (latency_target_fd == -1) {
		printf("Cannot set cpu_dma_latency\n");
		return;
	}

	errno = 0;
	int32_t latency_target_value = 0;
	err = write(latency_target_fd, &latency_target_value, 4);
	if (err < 1) {
		printf("nooo\n");
		close(latency_target_fd);
		return;
	}
}

/** TAKEN FOR rt-test **/
static inline void tsnorm(struct timespec *ts)
{
	while (ts->tv_nsec >= NSEC_PER_SEC) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}

/** TAKEN FOR rt-test **/
static inline int64_t calcdiff(struct timespec t1, struct timespec t2)
{
	int64_t diff;
	diff = USEC_PER_SEC * (long long)((int) t1.tv_sec - (int) t2.tv_sec);
	diff += ((int) t1.tv_nsec - (int) t2.tv_nsec) / 1000;
	return diff;
}

#ifdef TRACING
void no_tracing_write_event(int ev_id)
{
	switch(ev_id)
	{
		case NOBENCH_TEST_ID_COOP_SCHED:
			tracepoint(nobench_trace, no_begin_coop_sched);
			break;
		case NOBENCH_TEST_ID_COOP_SCHED + 1:
			tracepoint(nobench_trace, no_end_coop_sched);
			break;
		case NOBENCH_TEST_ID_SEM_INC:
			tracepoint(nobench_trace, no_begin_sem_inc);
			break;
		case NOBENCH_TEST_ID_SEM_INC + 1:
			tracepoint(nobench_trace, no_end_sem_inc);
			break;
		case NOBENCH_TEST_ID_SEM_SIGNAL_PRIO:
			tracepoint(nobench_trace, no_begin_sem_prio);
			break;
		case NOBENCH_TEST_ID_SEM_SIGNAL_PRIO + 1:
			tracepoint(nobench_trace, no_end_sem_prio);
			break;
		case NOBENCH_TEST_ID_MQ_SEND:
			tracepoint(nobench_trace, no_begin_mq_send);
			break;
		case NOBENCH_TEST_ID_MQ_SEND + 1:
			tracepoint(nobench_trace, no_end_mq_send);
			break;
	}
}

void no_tracing_report()
{

}
#endif
