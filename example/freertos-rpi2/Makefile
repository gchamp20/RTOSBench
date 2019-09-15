NOBENCH_PATH = ../..
WORKDIR = $(shell pwd)
FREERTOS_SRC_DIR =$(WORKDIR)/freertos/FreeRTOS/Source

OBJDIR = bld

ifeq ($(NOBENCH_TRACING), 2)
CFLAGS += -finstrument-functions \
		  -finstrument-functions-exclude-file-list=portASM,tests,tracing \
		  -finstrument-functions-exclude-file-list=printf,rpi_aux,rpi_base,rpi_gpio,rpi_i2c,rpi_systimer,startup \
		  -finstrument-functions-exclude-function-list=trace_switch_in,trace_switch_out,no_cycle_get_count,no_serial_write,rpi_aux_mu_string,sprintf \
		  -I $(NOBENCH_PATH)/tracing \
		  -DTRACING
VPATH += $(NOBENCH_PATH)/tracing:
TRACING_OBJS = $(OBJDIR)/cygprofile.o
else
ifeq ($(NOBENCH_TRACING), 1)
CFLAGS += -I $(NOBENCH_PATH)/tracing \
		  -DTRACING
VPATH += $(NOBENCH_PATH)/tracing:
TRACING_OBJS = $(OBJDIR)/trace.o
else
TRACING_OBJS =
endif
endif


CFLAGS += -nostdlib -nostartfiles -ffreestanding \
		  -I $(WORKDIR)/include \
		  -I $(FREERTOS_SRC_DIR)/include \
		  -I $(FREERTOS_SRC_DIR)/portable/GCC/ARM_CA7 \
		  -I $(FREERTOS_SRC_DIR)/../Demo/CORTEX_A7_BCM2836 \
		  -I $(NOBENCH_PATH)/include \
		  -D RPI2 \
		  -D NB_ITER=$(NO_NB_ITER) \
		  -march=armv7-a -mtune=cortex-a7 \
		  -MD \
		  -O2 -g

ALFLAGS += -mcpu=cortex-a7

VPATH +=$(FREERTOS_SRC_DIR):
VPATH +=$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CA7:
VPATH +=$(FREERTOS_SRC_DIR)/portable/MemMang:
VPATH +=$(FREERTOS_SRC_DIR)/../Demo/CORTEX_A7_BCM2836:
VPATH +=$(FREERTOS_SRC_DIR)/../Demo/CORTEX_A7_BCM2836/Drivers:

NOBENCH_OBJS = $(NO_TEST_SRC:.c=.o)
NOBENCH_TARGET = $(NO_TEST_SRC:.c=.elf)

OBJECTS = $(OBJDIR)/list.o \
		  $(OBJDIR)/queue.o \
		  $(OBJDIR)/tasks.o \
		  $(OBJDIR)/heap_4.o \
		  $(OBJDIR)/event_groups.o \
		  $(OBJDIR)/port.o  \
		  $(OBJDIR)/portASM.o  \
		  $(OBJDIR)/portISR.o  \
		  $(OBJDIR)/startup.o \
		  $(OBJDIR)/freertos_porting_layer.o \
		  $(OBJDIR)/mmu.o \
		  $(OBJDIR)/$(NOBENCH_OBJS) \
		  $(TRACING_OBJS)

RPI_DRIVER_OBJS = $(OBJDIR)/rpi_aux.o \
				  $(OBJDIR)/rpi_gpio.o \
				  $(OBJDIR)/rpi_i2c.o \
				  $(OBJDIR)/rpi_irq.o \
				  $(OBJDIR)/rpi_systimer.o \
				  $(OBJDIR)/printf.o \

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
LD = arm-none-eabi-ld

LIBGCC = $(abspath $(shell $(CC) -print-libgcc-file-name))
LIBC = $(abspath $(shell $(CC) -print-file-name=libc.a))

# The raspberry PI looks for kernel.img explicitly, do no change it unless modyfing config.txt accordingly.
all: kernel7.img

kernel7.img: kernel7.elf
	$(OBJCOPY) kernel7.elf -O binary $@

# Possibly mssing LIBGCC and libC?
kernel7.elf: $(OBJECTS) $(RPI_DRIVER_OBJS)
	$(LD) $^ $(LIBGCC) $(LIBC) -Map kernel7.map -o $@ -T $(FREERTOS_SRC_DIR)/../Demo/CORTEX_A7_BCM2836/raspberrypi.ld

include $(NOBENCH_PATH)/mk/common.mk
