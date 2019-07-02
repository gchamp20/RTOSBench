NOBENCH_PATH = ../..
LINUX_PORT_DIR := $(shell pwd)

OBJDIR = bld

ifeq ($(NOBENCH_TRACING), 1)
LIBS += -lrt -lpthread -lm -llttng-ust -ldl
CFLAGS += -D_GNU_SOURCE \
		  -I $(LINUX_PORT_DIR)/include \
		  -I $(LINUX_PORT_DIR)/tracing \
		  -I $(NOBENCH_PATH)/include -O3 \
		  -I $(NOBENCH_PATH)/tracing \
		  -DNB_ITER=$(NO_NB_ITER) \
		  -DBASE_PRIO=$(NO_BASE_PRIO) \
		  -DTRACING
LINUX_OBJS = $(OBJDIR)/tp.o $(OBJDIR)/linux_porting_layer.o
else
LIBS += -lrt -lpthread -lm -lc
CFLAGS += -D_GNU_SOURCE \
		  -I $(LINUX_PORT_DIR)/include \
		  -I $(NOBENCH_PATH)/include -O3 \
		  -DNB_ITER=$(NO_NB_ITER) \
		  -DBASE_PRIO=$(NO_BASE_PRIO) \
		  -DNO_AFFINITY=$(NO_AFFINITY)

LINUX_OBJS = $(OBJDIR)/linux_porting_layer.o
endif

ifdef DEBUG
CFLAGS += -g -O0
endif


TARGETS = $(NO_TEST_SRC:.c=)


VPATH += $(LINUX_PORT_DIR):
VPATH += $(LINUX_PORT_DIR)/tracing:
VPATH += $(NOBENCH_PATH)/src/tests/context-switch:
VPATH += $(NOBENCH_PATH)/src/tests/semaphore:
VPATH += $(NOBENCH_PATH)/src/tests/mutex:
VPATH += $(NOBENCH_PATH)/src/tests/latency:
VPATH += $(NOBENCH_PATH)/src/tests/event:
VPATH += $(NOBENCH_PATH)/src/tests/mq:
VPATH += $(NOBENCH_PATH)/src/lib/:

CC = gcc

.PHONY: all
#print:
#	@echo $(LINUX_OBJS)

all: $(OBJDIR) $(TARGETS)

$(TARGETS): % : $(LINUX_OBJS) $(OBJDIR)/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

include $(NOBENCH_PATH)/mk/common.mk
