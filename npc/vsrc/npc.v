import "DPI-C" function int inv_inst();
import "DPI-C" function int ebreak();
module npc(
	input		clk, rst,
	output	[31:0]reg_copy[15:0],
	output	[31:0]pc_copy
);

	wire jump;
	wire [31:0]jump_addr;
	wire [31:0]inst, pc;
	ifu IFU(
		.jump				(jump),
		.jump_addr	(jump_addr),
		.inst				(inst),
		.pc					(pc),
		.clk				(clk),
		.rst				(rst)
	);
	assign pc_copy = pc;

	wire [6:0]opcode, funct7;
	wire [2:0]funct3;
	wire [31:0]num1, num2, imm;
	wire [4:0]rd;
	wire [4:0]gpr_read_addr1, gpr_read_addr2;
	wire [31:0]gpr_read_data1, gpr_read_data2;
	idu IDU(
		.inst				(inst),
		.opcode			(opcode),
		.funct7			(funct7),
		.funct3			(funct3),
		.num1				(num1),
		.num2				(num2),
		.imm				(imm),
		.rd					(rd),
		.gpr_addr1	(gpr_read_addr1),
		.gpr_addr2	(gpr_read_addr2),
		.gpr_data1	(gpr_read_data1),
		.gpr_data2	(gpr_read_data2)
	);

	wire [4:0]gpr_write_addr;
	wire [31:0]gpr_write_data;
	wire lsu_read_en, lsu_write_en;
	wire [2:0]lsu_fmt;
	wire [31:0]lsu_addr, lsu_write_data;
	exu EXU(
		.opcode					(opcode),
		.funct3					(funct3),
		.funct7					(funct7),
		.num1						(num1),
		.num2						(num2),
		.imm						(imm),
		.rd							(rd),
		.gpr_write_addr	(gpr_write_addr),
		.gpr_write_data	(gpr_write_data),
		.lsu_read_en		(lsu_read_en),
		.lsu_write_en		(lsu_write_en),
		.lsu_fmt				(lsu_fmt),
		.lsu_addr				(lsu_addr),
		.lsu_write_data	(lsu_write_data),
		.pc							(pc),
		.jump						(jump),
		.jump_addr			(jump_addr)
	);

	wire [4:0]gpr_write_addr_o;
	wire [31:0]gpr_write_data_o;
	lsu LSU(
		.lsu_read_en		(lsu_read_en),
		.lsu_write_en		(lsu_write_en),
		.lsu_fmt				(lsu_fmt),
		.lsu_addr				(lsu_addr),
		.lsu_write_data	(lsu_write_data),
		.gpr_write_addr	(gpr_write_addr),
		.gpr_write_data	(gpr_write_data),
		.gpr_write_addr_o(gpr_write_addr_o),
		.gpr_write_data_o(gpr_write_data_o)
	);

	gpr GPR(
		.read_addr1	(gpr_read_addr1),
		.read_addr2	(gpr_read_addr2),
		.read_data1	(gpr_read_data1),
		.read_data2	(gpr_read_data2),
		.write_addr	(gpr_write_addr_o),
		.write_data	(gpr_write_data_o),
		.clk				(clk),
		.reg_copy		(reg_copy)
	);
	
endmodule
