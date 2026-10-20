module exu(
	input		[6:0]opcode,
	input		[2:0]funct3,
	input		[6:0]funct7,
	input		[31:0]num1, num2, imm,
	input		[4:0]rd,
	output	[4:0]gpr_write_addr,
	output	[31:0]gpr_write_data,
	output	lsu_read_en, lsu_write_en,
	output	[2:0]lsu_fmt,
	output	[31:0]lsu_addr, lsu_write_data,
	input		[31:0]pc,
	output	jump,
	output	[31:0]jump_addr,
	output	csr_write_en,
	output	[31:0]csr_write_data
);
	assign lsu_fmt = funct3;
	assign lsu_addr = num1 + imm;
	assign lsu_read_en = (opcode == 7'b0000011);
	assign lsu_write_en = (opcode == 7'b0100011);
	assign lsu_write_data = num2;

	assign gpr_write_addr = (opcode == 7'b0110011 ||												// calculate
		opcode == 7'b0010011 ||																								// cal-i
		opcode == 7'b1100111 ||																								// jalr
		opcode == 7'b1101111 ||																								// jal
		opcode == 7'b0000011 ||																								// load
		opcode == 7'b0010111 ||																								// auipc
		opcode == 7'b0110111 ||																								// lui
		opcode == 7'b1110011																									// system
		) ? rd : 0;

	assign gpr_write_data = (opcode == 7'b0110011) ? (
			(funct3 == 3'b000) ? (
				(funct7 == 7'b0000000) ? (num1 + num2) :													// add
				(funct7 == 7'b0100000) ? (num1 - num2) :													// sub 
				inv_inst()) :
			(funct3 == 3'b001) ? (num1 << num2[4:0]) :													// sll
			(funct3 == 3'b010) ? {31'd0, ($signed(num1) < $signed(num2))} :			// slt
			(funct3 == 3'b011) ? {31'd0, (num1 < num2)} :												// sltu
			(funct3 == 3'b100) ? (num1 ^ num2) :																// xor
			(funct3 == 3'b101) ? (
				(funct7 == 7'b0000000) ? (num1 >> num2[4:0]) :										// srl
				(funct7 == 7'b0100000) ? $signed(($signed(num1) >>> num2[4:0])) :	// sra
				inv_inst()) :
			(funct3 == 3'b110) ? (num1 | num2) :																// or
			(funct3 == 3'b111) ? (num1 & num2) :																// and
			inv_inst()) :
		(opcode == 7'b0010011) ? (
			(funct3 == 3'b000) ? (num1 + imm) :																	// addi
			(funct3 == 3'b001) ? (num1 << imm[4:0]) :														// slli
			(funct3 == 3'b010) ? {31'h0, {num1 - imm}[31]} :										// slti
			(funct3 == 3'b011) ? {31'h0, {{1'b0, num1} - {1'b0 , imm}}[32]} :		// sltiu
			(funct3 == 3'b100) ? (num1 ^ imm) :																	// xori
			(funct3 == 3'b101) ? (
				(funct7 == 7'b0000000) ? (num1 >> imm[4:0]) :											// srli
				(funct7 == 7'b0100000) ? $signed(($signed(num1) >>> imm[4:0])) :	// srai
				inv_inst()) :
			(funct3 == 3'b110) ? (num1 | imm) :																	// ori
			(funct3 == 3'b111) ? (num1 & imm) :																	// andi
			inv_inst()) :
		(opcode == 7'b1100111) ? (pc + 4) :																		// jalr
		(opcode == 7'b1101111) ? (pc + 4) :																		// jal
		(opcode == 7'b0010111) ? (pc + imm) :																	// auipc
		(opcode == 7'b0110111) ? imm :																				// lui
		(opcode == 7'b1110011 && funct3 == 3'd0 && imm == 32'd1) ? ebreak() :	// ebreak
		(opcode == 7'b0100011) ? 0 :																					// store
		(opcode == 7'b0000011) ? 0 :																					// load
		(opcode == 7'b1100011) ? 0 :																					// branch
		(opcode == 7'b1110011 && (funct3 == 1 || funct3 == 2)) ? num2 :				// csrr
		(opcode == 7'b1110011) ? 0 :
		inv_inst();
	
	assign jump_addr = (opcode == 7'b1100111) ? {{num1 + imm}[31:1], 1'd0} :// jalr
										 (opcode == 7'b1101111) ? (pc + imm) :								// jal
										 (opcode == 7'b1100011) ? (pc + imm) :								// branch
										 (opcode == 7'b1110011 && funct3 == 0 && imm == 0) ? num2 :// ecall
										 (opcode == 7'b1110011 && funct3 == 0 && imm == 32'h302) ? num2 : //mret
										 0;
	assign jump = (opcode == 7'b1100111) ? 1 :															// jalr
		(opcode == 7'b1101111) ? 1 :																					// jal
		(opcode == 7'b1100011) ? (
			(funct3 == 3'b000) ? (num1 == num2) :																// beq
			(funct3 == 3'b001) ? (num1 != num2) :																// bne
			(funct3 == 3'b100) ? ($signed(num1) < $signed(num2)) :							// blt
			(funct3 == 3'b110) ? (num1 < num2) :																// bltu
			(funct3 == 3'b101) ? ~($signed(num1) < $signed(num2)) :							// bge
			(funct3 == 3'b111) ? ~(num1 < num2) :																// bgeu
			inv_inst()[0]) : 
		(opcode == 7'b1110011 && funct3 == 0 && (imm == 0 || imm == 32'h302)) ? 1 :
		0;

	assign csr_write_en = (opcode == 7'b1110011) & ((funct3 == 3'b001) || (funct3 == 3'b010) || (funct3 == 0 && imm == 0));
	assign csr_write_data = (funct3 == 3'b001) ? num1 :
		(funct3 == 3'b010) ? (num1 | num2) :
		(funct3 == 0 && imm == 0) ? pc :
		0;
endmodule
