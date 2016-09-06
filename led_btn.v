module led_btn(leds, buttons);
	output [17:0] leds;
	input [3:0] buttons;

	assign leds = {buttons, 14'b0};
endmodule
