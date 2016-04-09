# 046267 Computer Architecture - Spring 2016 - HW #1
# makefile for test environment

SOURCES = sim_main.c sim_mem.c sim_core.c
EXTRA_DEPS = sim_api.h

all: sim_main

OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
$(info OBJECTS=$(OBJECTS))

sim_main: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

$(OBJECTS): %.o: %.c
	gcc -c -o $@ $^

.PHONY: clean
clean:
	rm -f sim_main $(OBJECTS)
