CFLAGS = -Wall -O $(CFLAGS_$@)
LDFLAGS = -Wall -O $(LDFLAGS_$@)
VPI_CFLAGS := $(shell iverilog-vpi --cflags)
CFLAGS_DE2.o = $(VPI_CFLAGS)

DE2.vpi: DE2.o
	iverilog-vpi $^

clean:
	rm -f *.o *.vpi *.vvp
