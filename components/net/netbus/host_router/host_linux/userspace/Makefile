.PHONY: all clean

all: blctl blctld

CC ?= gcc
CFLAGS := -Wall -Wextra -O2 -g -pthread
BLCTL_INCLUDE_DIRS := -I./

BLCTL_OBJS := blctl.o blctl_ota.o utils.o blctl_log.o

blctl: $(BLCTL_OBJS) blctl_main.o
	@$(info Linking $@)
	@$(CC) $(CFLAGS) $(BLCTL_INCLUDE_DIRS) -o $@ $^

blctld: $(BLCTL_OBJS) blctld_main.o
	@$(info Linking $@)
	@$(CC) $(CFLAGS) $(BLCTL_INCLUDE_DIRS) -o $@ $^

%.o: %.c
	@$(info CC $@)
	@$(CC) $(CFLAGS) $(BLCTL_INCLUDE_DIRS) -c -o $@ $<

clean:
	rm -f blctl blctld *.o
