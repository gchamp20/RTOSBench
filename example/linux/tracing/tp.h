#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER nobench_trace

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "tp.h"

#if !defined(_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _TP_H

#include <lttng/tracepoint.h>

/*
 * TRACEPOINT_EVENT(), TRACEPOINT_EVENT_CLASS(),
 * TRACEPOINT_EVENT_INSTANCE(), TRACEPOINT_LOGLEVEL(),
 * and `TRACEPOINT_ENUM()` are used here.
 */

TRACEPOINT_EVENT(
	nobench_trace,

	no_begin_coop_sched,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_end_coop_sched,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_begin_sem_inc,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_end_sem_inc,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_begin_sem_prio,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_end_sem_prio,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_begin_mq_send,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)

TRACEPOINT_EVENT(
	nobench_trace,

	no_end_mq_send,

	/* List of tracepoint arguments (input) */
	TP_ARGS(
	),

	/* List of fields of eventual event (output) */
	TP_FIELDS(
	)
)



#endif /* _TP_H */

#include <lttng/tracepoint-event.h>
