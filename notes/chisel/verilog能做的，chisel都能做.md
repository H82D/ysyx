模块定义：
```verilog
module GCD(
```
```scala
class GCD extends Module {
```
输入输出：
```verilog
input [15:0]value1, value2,
input loadingValues,
output [15:0]outputGCD,
output outputValid,
input clk
);
```
```scala
val io = IO(new Bundle {
	val value1 = Input(UInt(16.W))
	val value2 = Input(UInt(16.W))
	val loadingValues = Input(Bool())
	val outputGCD = Output(UInt(16.W))
	val outputValid = Output(Bool())
})
```
时序逻辑：(以下是“错误”示范，详见讲义“若干代码风格和规范”)
```verilog
reg [15:0]x, y;
always@(posedge clk) begin
	if(loadingValues) begin
		x <= value1;
		y <= value2;
	end
	else if(x > y)
		x <= x - y;
	else
		y <= y - x;
end
```
```scala
val x = Reg(UInt())
val y = Reg(UInt())
when(x > y) {
	x := x - y
}.otherwise{
	y := y - x
}
when(io.loadingValues) {
	x := io.value1
	y := io.value2
}
```
组合逻辑：
```verilog
assign outputGCD = x;
assign ouputValid = y == 16'b0;
endmodule
```
```scala
io.outputGCD := x
io.outputValid := y === 0.U
}
```