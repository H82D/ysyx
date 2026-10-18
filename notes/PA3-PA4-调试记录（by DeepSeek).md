# PA3 + PA4 调试记录

> 从 am-test intr 到 RT-Thread 启动的完整踩坑过程,以及每个 bug 的定位思路。

---

## PA3:am-test intr 与 difftest 的 sp 不一致

### 现象

nemu 单跑 intr 正常(不断输出 y),但与 spike difftest 时,**mret 指令 2 个周期后 sp 不一致**。

### 定位过程

1. 先看 difftest 到底比较了什么:`nemu/src/isa/riscv32/difftest/dut.c` 的 `isa_difftest_checkregs` **只比较 32 个 GPR,不比较 pc,也不比较 CSR**。所以报 "sp" 只说明 x0/x1 还相等、第一个出差异的通用寄存器是 x2,而 pc 可能早就分叉了。
2. "mret 后第 2 个周期才变" → sp 只会被 trap entry 的第一句 `addi sp, sp, -CONTEXT_SIZE` 改 → 说明第 2 个周期一方已经进了 trap entry、另一方还在执行普通指令,**pc 在 mret 之后就已经不同了**。
3. 检查 mret 实现:当时的实现是 `s->dnpc = cpu.csr[mepc] + 4`(ecall 也存的是 `pc + 4`)。而 spike 的 ecall/mret **都不加 4**。

### 根因与结论

RISC-V 里硬件是"笨"的、固定的:

- `ecall` 把 **ecall 自己的地址** 写进 mepc;
- `mret` 直接 `pc ← mepc`,不自动 +4。

"跳过 ecall" 是**软件(handler)的职责**:am 的 trap.S 不前进 mepc,所以 mret 会回到 ecall 本身、再次 trap,于是 intr 测试无限打印 y——这正是 spike 的行为。想让 yield() 正常返回,应该在 `__am_irq_handle` 的 `0xb` 分支写 `c->mepc += 4`。

顺带补的 mstatus(这次 difftest 不比较 CSR 所以没暴露,但下一阶段会咬人):

- `mret` 要更新 mstatus:MIE←MPIE、MPIE←1、特权级←MPP;
- `isa_raise_intr` 要更新 mstatus:MPIE←MIE、MIE←0、MPP←当前特权级。

### 教训

单跑 nemu "看起来对" 不代表对;difftest 只比 GPR 时,pc/CSR 的分叉会"延迟"到动 GPR 时才暴露。

---

## PA4:RT-Thread 启动调试

### 问题一:`_rt_mutex_take` 写 0 地址

**现象**:`sw a5, 4(a4)` 且 a4=0,写地址 4 越界。native 正常,riscv32-nemu 崩。

**定位链**(一层层回溯寄存器来源):

1. `sw a5, 4(a4)` 的基址 `a4 = 0`;
2. a4 来自 `lw a4, 76(s1)`;
3. s1 来自 `rt_thread_self()` 的返回值(0x82042768)。

**把偏移翻译成 C**:在当前配置下(`RT_NAME_MAX=24`、开 `RT_USING_MUTEX`/`RT_USING_EVENT`),`struct rt_thread` 的 **偏移 76 正好是 `taken_object_list.next`**。于是:

```c
lw a4, 76(s1)   // a4 = thread->taken_object_list.next (= NULL)
sw a5, 4(a4)    // l->next->prev = n  → 写 NULL+4 = 地址 4
```

这正是 `rt_list_insert_after(&thread->taken_object_list, &mutex->taken_list)` 的第一句。

**用 watchpoint 钉字段的写入历史**(这是整个 PA4 最有用的一招):

- 第一次: `_thread_init` 写自环(正常);
- 中间几次: `rt_mutex_take` / `rt_mutex_release` 增删链表(正常);
- 最后一次: **printf 的 memset 把它清零**。

→ 说明字段不是"忘了初始化",而是"被 memset 写坏"。

### 问题二:上下文切换不保存现场

在追 memset 的过程中,先发现了 `context.c` 的切换逻辑有问题:

- 原代码 `*(Context**)from = *(Context**)to` 把 **to 线程的 sp 写进 from 线程的 sp**;
- `ev_handler` 收到 trap.S 传来的现场指针 `c`,却直接丢弃。

**不变量**:线程 A 被切出时,它的现场被 trap.S 存在 `c`(位置随栈变化),必须写回 `A->sp`(`*(Context**)from = c`),否则 A 下次恢复会从错误的现场/栈继续跑,乱写内存。

**修复**:用 `chain[2]` 数组把 `from`/`to` 通过 `user_data` 传给 `ev_handler`,在 handler 里:

```c
if (from) *from = c;   // 保存被切出线程的现场
c = *to;               // 取目标线程的现场
```

注意第一次 `rt_hw_context_switch_to`(调度器启动)没有 from 需要保存,`from = 0` 时跳过保存。

### 问题三:printf 的 4KB 缓冲区栈溢出

**现象**:watchpoint 显示 memset 清零了 `taken_object_list.next`;memset 的调用者是 **printf**。

**参数**:`dst = 0x82041fc4`,`size = 4092`。缓冲区 `[0x82041fc4, 0x82042fc0)` 把线程结构体整个盖住了。

**根因**:

- am 的 printf(`abstract-machine/klib/src/stdio.c:11`)里是 `char buf[4096] = {0}`——**4KB 的栈上缓冲区**;
- 而 `RT_MAIN_THREAD_STACK_SIZE = 2048`(`rtconfig.h:41`),main 线程栈只有 2KB;
- 结构体 `[0x82042768, 0x82042828)` 和栈 `[0x82042828, 0x82043028)` 在堆里**紧挨着**;
- 4KB 缓冲区在 2KB 栈里必然溢出,向低地址冲出去,正好把紧挨栈底的结构体清零。

**修复**:减小 printf 的缓冲区(或把相关线程栈调大)。

**验证**:读结构体字段 `stack_addr`(偏移 56)/ `stack_size`(偏移 60)确认布局,发现 `stack_addr = 0x82042828 > 0x82042768`(结构体),并没有重叠——这反过来排除了"堆重叠",指向了"栈溢出"。

### 问题四:tcall 的 trampoline 数据被栈覆盖

**现象**:main 线程能启动了,但 `main_thread_entry` 返回后,`tcall` 应该调用 `_thread_exit`,却跳到了 `0x82045180`(栈区数据当指令执行)。

**根因**:`rt_hw_stack_init` 把 trampoline 数据 `{entry, param, exit}` 存在 **初始 sp 的下方**。线程一跑,栈帧向下增长就把这块数据覆盖了。而 `tcall` 里:

- `t->entry`、`t->param` 是在调用 entry **之前**读 → 安全;
- `t->exit` 是在 entry **返回之后**才读 → 此时原数据早被覆盖,读到栈帧里的垃圾。

**修复**:进 `tcall` 时先把 trampoline 结构体**拷贝到局部变量**(局部在 tcall 自己的栈帧里,位于 entry 栈帧上方,不会被覆盖):

```c
void tcall(void *thread) {
  trd t = *(trd*)thread;   // 拷贝到局部,而不是保留指向原地址的指针
  t.entry(t.param);
  t.exit();
}
```

---

## 调试方法小结

1. **difftest 分层排除**:native 正常只能排除"纯逻辑 bug",riscv 专属的 nemu/am 移植/rt-thread 移植都要分别怀疑。
2. **反汇编 + 寄存器回溯**:从崩溃指令的基址寄存器一路回溯到来源函数/字段。
3. **把偏移翻译成结构体字段**:崩溃地址是"哪个结构体的哪个字段"往往能直接定位语义。
4. **watchpoint 抓写坏的元凶**:对某个字段设 watchpoint,看"初始化(正常)→ 若干正常访问 → 某次非法写"的完整历史,直接揪出 memset 这种元凶。
5. **明确不变量**:现场保存(`c → from->sp`)、栈边界(`sp 不能越过栈底`)、trampoline 数据生命周期等,拿不变量去对照代码,往往一眼看出违反点。
6. **读结构体字段验证假设**:不要脑补内存布局,直接 `*(addr+offset)` 读出来确认。