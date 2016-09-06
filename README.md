## [DE2-114][DE2] board simulator using [SDL][SDL] for the UI and [Icarus Verilog][iverilog] through [VPI][VPI] as the simulation backend.

[DE2]: http://de2-115.terasic.com/
[SDL]: https://www.libsdl.org/
[iverilog]: http://iverilog.icarus.com/
[VPI]: https://en.wikipedia.org/wiki/Verilog_Procedural_Interface

### Installation

#### Dependencies

You need SDL2, SDL2_image, and icarus verilog.

To install those on OS X with MacPorts:

```
$ sudo port install iverilog libsdl2 libsdl2_image
```

#### Building

```
$ make
iverilog-vpi --name=DE2 -Wall -I/opt/local/include -L/opt/local/lib -lSDL2 -lSDL2_image  boardsim.c
Compiling boardsim.c...
Making DE2.vpi from  boardsim.o...
```

### Usage

A simple module that sets LEDs as a function of button & switch states:

```
$ cat io_test.v
module io_test(clk, leds, switches, buttons);
	input clk;
	output reg [17:0] leds;
	input [17:0] switches;
	input [3:0] buttons;

	always @(posedge clk) begin
		leds = switches & ~{buttons[1:0], buttons, buttons, buttons, buttons};
	end
endmodule
```

Start the simulator (from the top repo dir... becuase TODOs): `./board.sh top_module_name '.nets(passed), ...' ~/some/path/file1.v /tmp/whatever.v ...`

```
$ ./board.sh io_test '.clk(clk), .leds(leds), .switches(switches), .buttons(buttons)' io_test.v
```

Click the switches to change them, and press Q, W, E, & R on the keyboard to control the buttons.

![io_test simulation gif](io_test.gif)

## Resources

### VPI
#### Introductory
- [ASIC World PLI guide](http://www.asic-world.com/verilog/pli.html)
- [Use with Verilator](http://www.veripool.org/projects/verilator/wiki/manual-verilator#VERIFICATION-PROCEDURAL-INTERFACE-VPI)
- [Use with Icarus](http://iverilog.wikia.com/wiki/Using_VPI)

#### Reference
- [IEEE 1364-2005 (the Verilog standard)](http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=1620780) &mdash; sections 26 & 27 define VPI, Annex G is the normative vpi_user.h
- [Verilator's vpi_user.h](https://github.com/grg/verilator/blob/master/include/vltstd/vpi_user.h)
- [Icarus' vpi_user.h](https://github.com/steveicarus/iverilog/blob/master/vpi_user.h)
- [The Verilog PLI Handbook](https://books.google.com/books?id=LGTSBwAAQBAJ) (book)
- [Principles of Verilog PLI](https://books.google.com/books?id=VKXfBwAAQBAJ) (book)

### SDL
#### Introductory
- [Lazy Foo's SDL2 tutorial](http://lazyfoo.net/tutorials/SDL/)

#### Reference
- [Base SDL2 API reference](https://wiki.libsdl.org/CategoryAPI)
- [SDL_image API reference](https://www.libsdl.org/projects/SDL_image/docs/SDL_image.html)
