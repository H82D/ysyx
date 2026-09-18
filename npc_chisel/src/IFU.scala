package npc
import chisel3._

class IFU extends Module {
  val ifu_wbu = IO(new Bundle {
    val jump      = Input(UInt(1.W))
    val jump_addr = Input(UInt(1.W))
  })
  val ifu_ram = IO(new Bundle {
    val inst_addr = Output(UInt(32.W))
    val inst      = Input(UInt(32.W))
  })
  val ifu_idu = IO(new Bundle {
    val inst      = Output(UInt(32.W))
  })

  val pc = Reg(UInt(32.W))

  when(ifu_wbu.jump) {
    pc := ifu_wbu.jump_addr
  }.otherwise {
    pc := pc + 4
  }

  ifu_ram.inst_addr := pc
  ifu_idu.inst := ifu_ram.inst
}
