package npc
import chisel3._
import chisel3.util._

class IDU extends Module {
  val idu_ifu = IO(new Bundle {
    val inst = Input(UInt(32.W))
  })
  val idu_exu = IO(new Bundle {
    val opcode = Output(UInt(7.W))
    val funct3 = Output(UInt(3.W))
    val funct7 = Output(UInt(7.W))
    val num1 = Output(UInt(32.W))
    val num2 = Output(UInt(32.W))
    val rd = Output(UInt(5.W))
    val imm = Output(UInt(32.W))
  })
  val idu_gpr = IO(new Bundle {
    val read_en1 = Output(UInt(1.W))
    val read_addr1 = Output(UInt(5.W))
    val read_data1 = Input(UInt(32.W))
    val read_en2 = Output(UInt(1.W))
    val read_addr2 = Output(UInt(5.W))
    val read_data2 = Input(UInt(32.W))
  })

  val typ = Wire(UInt(3.W))
  val type_R :: type_I :: type_S :: type_U :: type_B :: type_J :: inv = Enum(6)

  val inst = idu_ifu.inst

  switch (inst(7, 0)) {
    is ("b01100_11".U) { typ = type_R }
    is ("b00100_11".U) { typ = type_I }
    is ("b00000_11".U) { typ = type_I }
    is ("b01000_11".U) { typ = type_S }
    is ("b11011_11".U) { typ = type_J }
    is ("b11000_11".U) { typ = type_B }
    is ("b00101_11".U) { typ = type_U }
    is ("b01101_11".U) { typ = type_U }
  }

  idu_exu.opcode := inst(6, 0)
  idu_exu.rd := inst(11, 7)
  idu_exu.funct3 := inst(14, 12)
  idu_exu.funct7 := inst(31, 25)

  switch (typ) {
    is (type_R) {
      idu_exu.num1 := idu_gpr.read_data1
      idu_exu.num2 := idu_gpr.read_data2
      idu_exu,imm := 0.U(32.W)
    }
    is (type_I) {
      idu_exu.num1 := idu_gpr.read_data1
      idu_exu.num2 := 0.U(32.W)
      idu_exu.imm := Cat(0.U(20.W), inst(31, 20))
    }
    is (type_S) {
      idu_exu.num1 := idu_gpr.read_data1
      idu_exu.num2 := idu_gpr.read_data2
      idu_exu.imm := Cat(0.U(20.W), inst(31, 25), inst(11, 7))
    }
    is (type_U) {
      idu_exu.num1 := 0.U(32.W)
      idu_exu.num2 := 0.U(32.W)
      idu_exu.imm := Cat(inst(31,12), 0.U(12.W))
    }
    is (type_B) {
      idu_exu.num1 := idu_gpr.read_data1
      idu_exu.num2 := idu_gpr.read_data2
      idu_exu.imm := Cat(0.U(19.W), inst(31), inst(7), inst(30, 25), inst(11, 8), 0.U(1.W))
    }
    is (type_J) {
      idu_exu.num1 := idu_gpr.read_data1
      idu_exu.num2 := idu_gpr.read_data2
      idu_exu.imm := Cat(0.U(11.W), inst(31), inst(19, 12), inst(20), inst(30, 21), 0.U(1.W))
    }
  }
}
