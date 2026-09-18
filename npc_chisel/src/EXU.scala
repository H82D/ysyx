package npc
import chisel3._

class EXU extends Module {
  val exu_idu = IO(new Bundle {
    val opcode = Input(UInt(7.W))
    val funct3 = Input(UInt(3.W))
    val funct7 = Input(UInt(7.W))
    val num1 = Input(UInt(32.W))
    val num2 = Input(UInt(32.W))
    val rd = Input(UInt(5.W))
    val imm = Input(UInt(32.W))
  })
  val exu_lsu = IO(new Bundle {
    val ram_read_en = Output(1.W)
    val ram_write_en = Output(1.W)
    val ram_data = Output(UInt(32.W))
    val ram_addr = Output(UInt(32.W))
    val gpr_addr = Output(UInt(5.W))
    val gpr_data = Output(UInt(32.W))
  }

  val opcode = exu_idu.opcode
  val funct3 = exu_idu.funct3
  val funct7 = exu_idu.funct7
  switch (opcode) {
    is ("b00100_11".U) {
      exu_lsu.ram_read_en = 0.U(1.W)
      exu_lsu.ram_write_en = 0.U(1.W)
      exu_lsu.ram_addr = 0.U(32.W)
      exu_lsu.ram_data = 0.U(32.W)
      exu_lsu.gpr_addr = exu_idu.rd
      switch(funct3) {
        is ("b000".U) {
          exu_lsu.gpr_data = exu_idu.num1 + exu_idu.imm
        }
        is ("b001".U) {
          when (exu_idu.funct7 === "b0000000".U) {
            exu_lsu.gpr_data = exu_idu.num1 << exu_idu.imm
          }.othwerwise {
            //TODO: invalid
          }
        }
        is ("b010".U) {
          exu_lsu.gpr_data = Cat(0.U(31.W), exu_idu.num1.asTypeOf(SInt) < exu_idu.imm.asTypeOf(SInt))
        }
        is ("b011".U) {
          exu_lsu.gpr_data = Cat(0.U(31.W), exu_idu.num1 < exu_idu.imm)
        }

