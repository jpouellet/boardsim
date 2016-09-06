module led_sw(leds, switches);
	output [17:0] leds;
	input [17:0] switches;

	assign leds = switches;
endmodule
