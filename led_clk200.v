module led_clk200(leds);
	output reg [17:0] leds;

	initial begin
		leds = 18'b1;
		forever #200 leds = ~leds;
	end
endmodule
