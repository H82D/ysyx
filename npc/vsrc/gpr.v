import "DPI-C" function void inv_reg();
module gpr(
	input		[4:0]read_addr1, read_addr2,
	output	[31:0]read_data1, read_data2,
	input		[4:0]write_addr,
	input		[31:0]write_data,
	input		clk,
	output	[31:0]reg_copy[15:0]
);
	reg	[31:0]reg_array[15:0];

	integer i;

	always@(posedge clk) begin
		reg_array[write_addr[3:0]] <= write_data;
	end

	assign read_data1 = (read_addr1 == 0) ? 0 : reg_array[read_addr1[3:0]];
	assign read_data2 = (read_addr2 == 0) ? 0 : reg_array[read_addr2[3:0]];
	assign reg_copy[15:1] = reg_array[15:1];
	assign reg_copy[0] = 0;
endmodule
