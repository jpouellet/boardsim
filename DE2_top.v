module DE2_top();
	wire [17:0] leds;
	reg [17:0] switches;
	reg [3:0] buttons;
	reg clk;

//TOPMODULE

	initial begin
		#5 clk = 1'b0;
		forever begin
			#10 clk = ~clk;
		end
	end

	initial forever begin
		#50;
		switches = $DE2_switches;
		buttons = $DE2_buttons;
		$DE2_leds(leds);
	end
endmodule
