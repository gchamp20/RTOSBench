ifndef EXAMPLE
EXAMPLE = linux
endif

ifndef NB_ITER
NB_ITER = 5000
endif

ifndef BASE_PRIO
BASE_PRIO = 10
endif

ifndef AFFINITY
AFFINITY = 7
endif

.PHONY: clean

clean:
	make -C example/$(EXAMPLE) clean

interrupt_processing:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=interrupt_processing.c NO_TEST_PATH=tests/interrupt NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

event:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=event.c NO_TEST_PATH=tests/event NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

event_processing:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=event_processing.c NO_TEST_PATH=tests/event NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mq:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mq.c NO_TEST_PATH=tests/mq NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mq_workload:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mq_workload.c NO_TEST_PATH=tests/mq NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mq_processing:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mq_processing.c NO_TEST_PATH=tests/mq NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mutex:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mutex.c NO_TEST_PATH=tests/mutex NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mutex_pip:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mutex_pip.c NO_TEST_PATH=tests/mutex NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mutex_workload:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mutex_workload.c NO_TEST_PATH=tests/mutex NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

mutex_processing:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=mutex_processing.c NO_TEST_PATH=tests/mutex NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

round_robin:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=round_robin.c NO_TEST_PATH=tests/context-switch NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

sched_latency:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=sched_latency.c NO_TEST_PATH=tests/latency NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

sem:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=sem.c NO_TEST_PATH=tests/semaphore NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

sem_processing:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=sem_processing.c NO_TEST_PATH=tests/semaphore NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

sem_prio:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=sem_prio.c NO_TEST_PATH=tests/semaphore NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

sem_workload:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=sem_workload.c NO_TEST_PATH=tests/semaphore NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

jitter:
	@make -C example/$(EXAMPLE) NO_TEST_SRC=jitter.c NO_TEST_PATH=tests/partition NO_NB_ITER=$(NB_ITER) NOBENCH_TRACING=$(TRACING) NO_BASE_PRIO=$(BASE_PRIO) NO_AFFINITY=$(AFFINITY)

