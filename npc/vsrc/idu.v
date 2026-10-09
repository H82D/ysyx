module idu(
	input		[31:0]inst,
	output	[6:0]opcode,
	output	[6:0]funct7,
	output	[2:0]funct3,
	output	[31:0]num1,
	output	[31:0]num2,
	output	[31:0]imm,
	output	[4:0]rd,
	output	[4:0]gpr_addr1, gpr_addr2,
	input		[31:0]gpr_data1, gpr_data2
);

	localparam R_type = 3'h0;
	localparam I_type = 3'h2;
	localparam S_type = 3'h4;
	localparam B_type = 3'h5;
	localparam U_type = 3'h6;
	localparam J_type = 3'h7;

	wire [2:0]typ;
	assign typ = (opcode == 7'b0110111) ? U_type :
		(opcode == 7'b0010111) ? U_type :
		(opcode == 7'b1101111) ? J_type :
		(opcode == 7'b1100111) ? I_type :
		(opcode == 7'b1100011) ? B_type :
		(opcode == 7'b0000011) ? I_type :
		(opcode == 7'b0100011) ? S_type :
		(opcode == 7'b0010011) ? I_type :
		(opcode == 7'b0110011) ? R_type :
		(opcode == 7'b0001111) ? I_type :
		(opcode == 7'b1110011) ? I_type :
		inv_inst()[2:0];

	assign opcode = inst[6:0];
	assign funct3 = inst[14:12];
	assign funct7 = inst[31:25];
	assign rd = inst[11:7];
	assign gpr_addr1 = inst[19:15];
	assign gpr_addr2 = inst[24:20];
	assign num1 = gpr_data1;
	assign num2 = gpr_data2;

	assign imm = (typ == I_type) ? {{20{inst[31]}}, inst[31:20]} :
		(typ == S_type) ? {{20{inst[31]}}, inst[31:25], inst[11:7]} :
		(typ == B_type) ? {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0} :
		(typ == U_type) ? {inst[31:12], 12'h0} :
		(typ == J_type) ? {{12{inst[31]}}, inst[19:12], inst[20], inst[30:21], 1'h0} :
		0;
endmodule
