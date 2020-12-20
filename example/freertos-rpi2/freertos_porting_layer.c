#include <string.h>
#include "porting_layer.h"

#include "Drivers/rpi_timer.h"
#include "Drivers/rpi_aux.h"
#include "Drivers/rpi_gpio.h"
#include "Drivers/rpi_irq.h"
#include "Drivers/printf.h"
#include "mmu.h"

#ifdef TRACING
#include "trace.h"
#endif

volatile unsigned int* coreTimer = (volatile unsigned int*)0x40000000;
volatile unsigned int* coreTimerPrescaler = (volatile unsigned int*)0x40000008;
volatile unsigned int* coreTimerLow = (volatile unsigned int*)0x4000001C;
volatile unsigned int* coreTimerHigh = (volatile unsigned int*)0x40000020;

volatile unsigned int* systemTimer = (volatile unsigned int*)0x3F003004;

volatile unsigned int* coreMailboxInterruptCtrl = (volatile unsigned int*)0x40000050;
volatile unsigned int* coreMailboxInterrupt = (volatile unsigned int*)0x40000080;
volatile unsigned int* coreMailboxInterruptClr = (volatile unsigned int*)0x400000C0;

char printfbuf[100];

static int task_ids = 100;

void no_initialize_test(no_task_entry_t init_test_fct)
{
	unsigned int ra;

	rpi_cpu_irq_disable();
	rpi_gpio_sel_fun(47, 1);
	rpi_gpio_sel_fun(35, 1);
	rpi_gpio_sel_fun(40, 1);
	rpi_gpio_sel_fun(21, 1);
	rpi_gpio_sel_fun(17, 1);
	rpi_gpio_sel_fun(14, 1);

	/* Enable JTAG connections */
	rpi_gpio_sel_fun(5, GPIO_FSEL_ALT5);
	rpi_gpio_sel_fun(6, GPIO_FSEL_ALT5);
	rpi_gpio_sel_fun(13, GPIO_FSEL_ALT5);
	rpi_gpio_sel_fun(12, GPIO_FSEL_ALT5);

	rpi_gpio_sel_fun(22, GPIO_FSEL_ALT4);
	rpi_gpio_sel_fun(26, GPIO_FSEL_ALT4);

	init_page_table();
	mmu_init();

	rpi_aux_mu_init();

#ifdef TRACING
	clear_trace_buffer();
	start_trace();
#endif

	init_test_fct(NULL);

	vTaskStartScheduler();
}

no_task_handle_t no_create_task(no_task_entry_t task_entry, char task_name[4], unsigned int prio)
{
	no_task_handle_t thandle;
	xTaskCreate(task_entry, (const char* const)task_name, 128, NULL,
			prio, &thandle);

	return thandle;
}

void no_task_yield()
{
	taskYIELD();
}

void no_task_suspend(no_task_handle_t task)
{
	vTaskSuspend(task);
}

void no_task_suspend_self()
{
	vTaskSuspend(NULL);
}

void no_task_resume(no_task_handle_t task)
{
	vTaskResume(task);
}

void no_task_delay(unsigned int milliseconds)
{
	vTaskDelay(milliseconds);
}

void no_init_timer()
{
	/* Clock timer on APB clock (Half arm speed, 40MHz (from experiment) ??) */
	*coreTimer = (1 << 7);

	/* This is equivalent to setting prescaler to 1 */
	*coreTimerPrescaler = 0x80000000;
	asm volatile ("wfe" : :);
}

void no_disable_timer()
{
}

void no_reset_timer()
{
}

no_time_t no_add_times(const no_time_t* base, unsigned int milliseconds)
{
	no_time_t retval = ((*base) + milliseconds * 900000);
	rpi_aux_mu_string(printfbuf);
	return retval;
}

no_time_t no_time_get()
{
	unsigned cc;
	asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r" (cc));
	return cc;
}

long no_time_diff(const no_time_t* t1, const no_time_t* t2)
{
	long diff = ((long)*t2 - (long)*t1);
	if (diff < 0)
	{
		diff = -diff;
	}
	return diff;
}

void no_sem_create(no_sem_t* sem, int current_value)
{
	*sem = xSemaphoreCreateCounting(50, current_value);
}

void no_sem_wait(no_sem_t* sem)
{
	if (xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE)
	{
		no_serial_write("sem_wait: error");
	}
}

void no_sem_signal(no_sem_t* sem)
{
	if (xSemaphoreGive(*sem) != pdTRUE)
	{
		no_serial_write("sem_signal: error");
	}
}

void no_mutex_create(no_mutex_t* mutex)
{
	*mutex = xSemaphoreCreateMutex();
}

void no_mutex_acquire(no_mutex_t* mutex)
{
	if (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE) {
		no_serial_write("mutex_acquire: fail");
	}
}

void no_mutex_release(no_mutex_t* mutex)
{
	if (xSemaphoreGive(*mutex) != pdTRUE) {
		no_serial_write("mutex_release: fail");
	}
}

void no_event_create(no_event_t* event)
{
	*event = xEventGroupCreate();
}

void no_event_set(no_event_t* event)
{
	xEventGroupSetBits(*event, 0x08);
}

void no_event_reset(no_event_t* event)
{
	xEventGroupClearBits(*event, 0x08);
}

void no_event_wait(no_event_t* event)
{
	xEventGroupWaitBits(*event, 0x08, pdTRUE, pdTRUE, portMAX_DELAY);
}

void no_mq_create(no_mq_t* mq, unsigned int length, unsigned int msgsize)
{
	*mq = xQueueCreate(length, msgsize);
}

void no_mq_send(no_mq_t* mq, unsigned int msg)
{
	xQueueSend(*mq, (void*)&msg, portMAX_DELAY);
}

void no_mq_receive(no_mq_t* mq)
{
	unsigned int rcv;
	xQueueReceive(*mq, &rcv, portMAX_DELAY);
}

void no_serial_write(const char* string)
{
	rpi_aux_mu_string((char*)string);
}

void no_report_results(long elapsed, long per_loop_overhead, long loop_overhead, int iteration)
{
	memset(printfbuf, '\0', 100);
	long time = ((elapsed - loop_overhead) / iteration) - per_loop_overhead;
	sprintf(printfbuf, "elapsed=%lu\nper_loop_overhead=%lu\nloop_overhead=%lu\niteration=%d\ntime=%lu",
			elapsed, per_loop_overhead, loop_overhead, iteration, time);
	rpi_aux_mu_string(printfbuf);
}

void no_reset_all_pmu_counter()
{
	unsigned int pmcr;
	asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r" (pmcr));
	pmcr |= (1 << 1);
	asm volatile ("mcr p15, 0, %0, c9, c12, 0" : : "r" (pmcr));
}

unsigned int no_inst_get_count()
{
	unsigned int cc;
	unsigned int reg = 0x0;
	/* PMSELR */
	asm volatile ("mcr p15, 0, %0, c9, c12, 5" : : "r" (reg));
	/* READ COUNT */
	asm volatile ("mrc p15, 0, %0, c9, c13, 2" : "=r" (cc));
	return cc;
}

void no_cycle_reset_counter()
{
	unsigned int pmcr;
	asm volatile ("mrc p15, 0, %0, c9, c12, 0" : "=r" (pmcr));
	pmcr |= (1 << 2);
	asm volatile ("mcr p15, 0, %0, c9, c12, 0" : : "r" (pmcr));
}

unsigned int no_cycle_get_count()
{
	unsigned int cc;
	asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r" (cc));
	return cc;
}

void no_single_result_report(char* prefix, int64_t time)
{
	memset(printfbuf, '\0', 100);
	int32_t test = (int32_t)time;
	sprintf(printfbuf, "%s%d", prefix, test);
	rpi_aux_mu_string(printfbuf);
}

void no_result_report(int64_t max, int64_t min, int64_t average)
{
	memset(printfbuf, '\0', 100);
	int32_t a, b, c;
	a = (int32_t)max;
	b = (int32_t)min;
	c = (int32_t)average;
	sprintf(printfbuf, "max=%d\nmin=%d\naverage=%d", a, b, c);
	rpi_aux_mu_string(printfbuf);
}

void no_interrupt_do_sgi()
{
	// Trigger core0 interrupt
	*coreMailboxInterrupt = 0x1;
}

void no_interrupt_enable_sgi()
{
	*coreMailboxInterruptCtrl = 0x1;
}

void no_interrupt_register_sgi_handler(no_int_handler_t fn)
{
	rpi_irq_register_handler(RPI_TOTAL_IRQ, fn, NULL);
}

#ifdef TRACING

void trace_switch_in(int tid)
{
	no_tracing_switch_in(tid);

	//memset(printfbuf, '\0', 100);
	//sprintf(printfbuf, "in tracesize=%d\n", no_tracing_status.idx);
	//rpi_aux_mu_string(printfbuf);
}

void trace_switch_out(int tid)
{
	no_tracing_switch_out(tid);

	//memset(printfbuf, '\0', 100);
	//sprintf(printfbuf, "out tracesize=%d\n", no_tracing_status.idx);
	//rpi_aux_mu_string(printfbuf);
}

void trace_switch_ready(int tid)
{
	no_tracing_switch_ready(tid);

	//memset(printfbuf, '\0', 100);
	//sprintf(printfbuf, "ready tracesize=%d\n", no_tracing_status.idx);
	//rpi_aux_mu_string(printfbuf);
}

void trace_task_create(no_task_handle_t task)
{
	vTaskSetTaskNumber(task, task_ids);
	task_ids++;
}

extern char no_trace_buffer[];
extern int32_t tracing_on;

void no_tracing_report()
{
	no_tracing_status.on = 0;
	int i;
	int* buf_ptr = (int*)no_trace_buffer;

	memset(printfbuf, '\0', 100);
	int int_size = no_tracing_status.idx >> 2;
	sprintf(printfbuf, "tracesize=%d\n", no_tracing_status.idx);
	rpi_aux_mu_string(printfbuf);
	for (i = 0; i < no_tracing_status.idx; i++)
	{
		rpi_aux_mu_raw_putc(no_trace_buffer[i]);
	}
	no_serial_write("Trace transmitted");
	no_tracing_status.on = 1;
}

void clear_trace_buffer()
{
	int i;
	for (i = 0; i < NO_BUFFER_SIZE; i++)
	{
		no_trace_buffer[i] = 0;
	}
}

void vApplicationIRQHandler();

void start_trace()
{
	rpi_aux_mu_string("TRACING ON\n");
	no_cycle_reset_counter();

	no_tracing_status.on = 1;
	no_tracing_status.idx = 0;
}

#endif
