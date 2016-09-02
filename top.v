module DE2_top();
	wire [17:0] leds;
	reg clk;

//TOPMODULE
count_test count_test (.clk(clk), .leds(leds));

	initial begin
		#5 clk = 1'b0;
		forever begin
			#10 clk = ~clk;
		end
	end

	initial forever begin
		#50 $DE2_leds(leds);
	end
endmodule
