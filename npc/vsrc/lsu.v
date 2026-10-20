import "DPI-C" function int ram_read(bit en, int addr);
import "DPI-C" function bit ram_write(bit en, int addr, int data, int len);
module lsu(
	input		lsu_read_en, lsu_write_en,
	input		[2:0]lsu_fmt,
	input		[31:0]lsu_addr, lsu_write_data,
	input		[4:0]gpr_write_addr,
	input		[31:0]gpr_write_data,
	output	[4:0]gpr_write_addr_o,
	output	[31:0]gpr_write_data_o,
	output	write_success
);
	assign gpr_write_addr_o = gpr_write_addr;
	wire [31:0]ram_read_data;
	assign ram_read_data = ram_read(lsu_read_en, lsu_addr);
	assign gpr_write_data_o = lsu_read_en ? (
		(lsu_fmt == 0) ? {{24{ram_read_data[7]}}, ram_read_data[7:0]} :
		(lsu_fmt == 4) ? {24'd0, ram_read_data[7:0]} :
		(lsu_fmt == 1) ? {{16{ram_read_data[15]}}, ram_read_data[15:0]} :
		(lsu_fmt == 5) ? {16'd0, ram_read_data[15:0]} :
		(lsu_fmt == 2) ? ram_read_data :
		inv_inst() ) : gpr_write_data; 
	
	wire [2:0]write_len;
	assign write_len = (lsu_fmt == 0) ? 1 :
		(lsu_fmt == 1) ? 2 :
		(lsu_fmt == 2) ? 4 : 3;
	assign write_success = ram_write(lsu_write_en, lsu_addr, lsu_write_data, {29'd0, write_len});
endmodule
