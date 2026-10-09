import "DPI-C" function int inst_fetch(int pc);
module ifu (
	input		jump,
	input		[31:0]jump_addr,
	output	[31:0]inst,
	output	[31:0]pc,
	input		clk, rst
);
	
	reg [31:0]pc_r;
	wire [31:0]pc_next;
	assign pc_next = rst ? 32'h8000_0000 : jump ? jump_addr : (pc_r + 4);

	always @(posedge clk or posedge rst) begin
		if (rst)
			pc_r <= 32'h8000_0000;
		else
			pc_r <= pc_next;
	end

	assign inst = inst_fetch(pc_r);
	assign pc = pc_r;
endmodule
