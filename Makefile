CPPFLAGS=-I/opt/local/include
CFLAGS=-Wall $(CPPFLAGS)
LDADD=-lSDL2 -lSDL2_image
LDFLAGS=-L/opt/local/lib $(LDADD)

DE2.vpi: boardsim.c
	iverilog-vpi --name=DE2 $(CFLAGS) $(LDFLAGS) $(LIBS) $^

clean:
	rm -f *.o *.vpi *.vvp
