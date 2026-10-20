module csr (
	input csr_write_en,
	input [11:0]csr_read_addr, csr_write_addr,
	input [31:0]csr_write_data,
	output[31:0]csr_read_data,
	input clk, rst,
	output [31:0]csr_copy[7:0]
);

reg [31:0]csr[7:0];
parameter mcycle = 4;
parameter mcycleh = 5;
parameter mvendorid = 6;
parameter marchid = 7;
parameter mstatus = 0;
parameter mepc = 1;
parameter mcause = 2;
parameter mtvec = 3;

wire [2:0]csr_read_idx;
wire [1:0]csr_write_idx;
assign csr_write_idx = (csr_write_addr == 12'h305) ? mtvec :
	(csr_write_addr == 12'h300) ? mstatus :
	(csr_write_addr == 12'h341) ? mepc :
	(csr_write_addr == 12'h342) ? mcause :
	0;

assign csr_read_idx = (csr_read_addr == 12'hb00) ? mcycle :
	(csr_read_addr == 12'hc80) ? mcycleh :
	(csr_read_addr == 12'hf11) ? mvendorid :
	(csr_read_addr == 12'hf12) ? marchid :
	(csr_read_addr == 12'h305) ? mtvec :
	(csr_read_addr == 12'h300) ? mstatus :
	(csr_read_addr == 12'h341) ? mepc :
	(csr_read_addr == 12'h342) ? mcause :
	0;

always @(posedge clk or posedge rst) begin
	if (rst) begin
		csr[mcycle]  <= 0;
		csr[mcycleh] <= 0;
		csr[mvendorid]  <= 32'h7973_7978;
		csr[marchid] <= 32'd26080222;
		csr[mtvec] <= 0;
		csr[mstatus] <= 32'h1800;
		csr[mepc] <= 0;
		csr[mcause] <= 32'hb;
	end
	else begin
		{csr[mcycle], csr[mcycleh]} <= {csr[mcycle], csr[mcycleh]} + 1;
		csr[{1'b0, csr_write_idx[1:0]}] <= csr_write_en ? csr_write_data : csr[{1'b0, csr_write_idx[1:0]}];
	end
end

assign csr_read_data = csr[csr_read_idx];
assign csr_copy = csr;

endmodule
