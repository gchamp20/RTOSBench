CLEANUP=*.o .depend *.*~ *.orig *.rej *.d
LIBNOBENCHOBJS = $(addprefix $(OBJDIR)/,string-manip.o results.o)
LIBNOBENCHFLAGS = -lnobench -L$(OBJDIR)

VPATH += $(NOBENCH_PATH)/tests/context-switch:
VPATH += $(NOBENCH_PATH)/tests/semaphore:
VPATH += $(NOBENCH_PATH)/tests/mutex:
VPATH += $(NOBENCH_PATH)/tests/latency:
VPATH += $(NOBENCH_PATH)/tests/event:
VPATH += $(NOBENCH_PATH)/tests/mq:
VPATH += $(NOBENCH_PATH)/tests/partition:
VPATH += $(NOBENCH_PATH)/tests/interrupt:
VPATH += $(NOBENCH_PATH)/tests/dhrystone:

clean:
	for F in $(CLEANUP); do find -type f -name $$F | xargs rm -f; done
	rm -rf bld

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) -c -Dgcc $< $(CFLAGS) -o $@

$(OBJDIR)/%.o: %.S | $(OBJDIR)
	$(CC) -c -Dgcc $< $(ALFLAGS) -o $@

$(OBJDIR)/%.o: %.s | $(OBJDIR)
	$(CC) -c -Dgcc $< $(ALFLAGS) -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/libnobench.a: $(LIBNOBENCHOBJS)
	$(AR) rcs $@ $^
