module io_test(clk, leds, switches, buttons);
	input clk;
	output reg [17:0] leds;
	input [17:0] switches;
	input [3:0] buttons;

	always @(posedge clk) begin
		leds = switches & ~{buttons[1:0], buttons, buttons, buttons, buttons};
	end
endmodule
