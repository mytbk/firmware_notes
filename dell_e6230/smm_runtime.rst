神秘的 SMM Runtime 协议
===========================

找到了启用 ACPI 的函数后，我接着看这个函数的内容::

  / (fcn) acpi_enable 77
  |   acpi_enable (int arg_40h);
  |           ; arg int arg_40h @ rsp+0x40
  |           ; DATA XREF from acpi_enable_disable (0x180000499)
  |           0x1800003ec      4883ec28       sub rsp, 0x28              ; '('
  |           0x1800003f0      803d45090000.  cmp byte [0x180000d3c], 0
  |       ,=< 0x1800003f7      753b           jne 0x180000434
  |       |   0x1800003f9      ba70000000     mov edx, 0x70              ; 'p' ; 112
  |       |   0x1800003fe      b00d           mov al, 0xd                ; 13
  |       |   0x180000400      488d0d390a00.  lea rcx, [0x180000e40]
  |       |   0x180000407      ee             out dx, al
  |       |   0x180000408      33c0           xor eax, eax
  |       |   0x18000040a      ba71000000     mov edx, 0x71              ; 'q' ; 113
  |       |   0x18000040f      ee             out dx, al
  |       |   0x180000410      488b05110b00.  mov rax, qword [0x180000f28] ; [0x180000f28:8]=0
  |       |   0x180000417      48c744244001.  mov qword [arg_40h], 1
  |       |   0x180000420      4c8b4010       mov r8, qword [rax + 0x10] ; [0x10:8]=-1 ; 16
  |       |   0x180000424      488d542440     lea rdx, [arg_40h]         ; 0x40 ; '@' ; 64
  |       |   0x180000429      41ff5010       call qword [r8 + 0x10]     ; 16
  |       |   0x18000042d      c60508090000.  mov byte [0x180000d3c], 1  ; [0x180000d3c:1]=0
  |       `-> 0x180000434      4883c428       add rsp, 0x28              ; '('
  \           0x180000438      c3             ret
  
里面有些陌生的全局变量。0x180000d3c 比较容易理解，就是标记 ACPI 是否已启用，防止重复初始化。有 0x70 和 0x71 两个端口的 out 指令，这都是和 CMOS/RTC 相关的，不是我关心的内容。最奇怪的是那个 ``call qword [r8+0x10]`` 的指令，这到底来自一个什么协议？

还是使用 axt 命令找交叉引用，把写了 0x180000f28 的函数找出来::

  | |||| ||   0x180000c10      4c8b1d090300.  mov r11, qword [0x180000f20] ; protocol_a56897a1 ; [0x180000f20:8]=0
  | |||| ||   0x180000c17      4d85db         test r11, r11
  | ||||,===< 0x180000c1a      7413           je 0x180000c2f
  | |||||||   0x180000c1c      4c8d05050300.  lea r8, [0x180000f28]
  | |||||||   0x180000c23      488d0db60200.  lea rcx, [0x180000ee0]
  | |||||||   0x180000c2a      33d2           xor edx, edx
  | |||||||   0x180000c2c      41ff13         call qword [r11]
  
看样子是这个调用写了 0x180000f28 的值，它来自于 0x180000f20 指向的协议，继续找交叉引用，发现它来自一个 GUID 为 a56897a1-a77f-4600-... 的协议，在网上搜了一些，看样子不少私有 UEFI 固件都有这个协议。

利用 UEFITool 和 radare2 的静态分析，很容易找到这个协议由 SmmRuntime 这个模块安装的，但是再分析这个模块，感觉就有点乱了。

之后在网上做了些搜索，在搜关键词 ``SmmRuntime`` 的时候，无意中搜到了一份代码，它来自于 Quark SoC 的固件 `Quark EDK2 <https://kernel.googlesource.com/pub/scm/linux/kernel/git/jejb/Quark_EDKII/+/master/QuarkSocPkg/QuarkNorthCluster/Smm/Dxe/SmmRuntime/SmmRuntime.c>`__.

我把这份代码所在的仓库整个 clone 了下来，看了下代码，不但发现里面部分代码和 SmmRuntime 里面的一些代码结构相似，而且发现了 a56897a1 开头的这个 GUID，它对应的协议叫 EFI SMM Runtime Protocol. 于是接下来我也明白了上面汇编代码里面的 ``call qword [r11]`` 实际上调的是 SMM Runtime Protocol 中的 LocateProtocol，和 Boot Service 的 LocateProtocol 相似。

顺便提一下，我怀疑固件中还有不少 SMM 相关的代码来自于 Quark SoC 的 EDK2 的代码。真正启用和禁用 ACPI 的 SMI 命令 0xa0 和 0xa1，和 QuarkPlatformPkg 里定义的完全一样。

最后做个总结，很多私有 UEFI 固件里面这个 GUID 为 a56897a1-... 的协议，是 SMM Runtime Protocol. 这个似乎不是一个标准的协议，在 SMM CIS 的文档中没找到，在上游 EDK2 的代码里也没有找到，却在 Quark SoC 的 EDK2 源码里找到了。不得不说，我这次非常幸运，找到了现成的代码，可以让我免去很多逆向分析的工作。
