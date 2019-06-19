Clevo W650SZ EC ACPI 支持的逆向
====================================

经过这两周断断续续的工作，我终于把蓝天 W650SZ 的 EC ACPI 支持相关的代码逆向出来了。

首先，把 coreboot 移植到 W650SZ 并不困难，用添加了 Haswell 平台支持的 autoport 可以生成一份可用的代码。和其他的笔记本一样，难点在于 EC 的支持上。而这台笔记本上的 EC 不需要做特殊的初始化操作，键盘和触摸板就能工作，风扇和温度控制也能用，因此只剩下 ACPI 的问题。

关于 ACPI 的支持，我在做 `Dell Latitude E6230 <../dell_e6230/dell_acpi.rst>`__ 的工作的时候提到过一下，操作系统启用 ACPI 的时候，一部分代码是在 SMM 中运行的固件的代码。我们需要在固件中找使用了 SMM_SW_DISPATCH_PROTOCOL 的模块，但是在这个机器上有点不同，它启用 ACPI 支持的模块在 AcpiModeEnable 中，使用的协议是 SMM_SW_DISPATCH2_PROTOCOL，其 GUID 和 MM_SW_DISPATCH_PROTOCOL 相同。不过两个版本的 SMM 协议用法很接近，很容易就找到了触发启用 ACPI 的 SMI 时，所要执行的函数。

在这个函数中，前面的部分访问了 0x1800 后面的一些端口，和 EC 的 0x62/0x66 关系不大，而且 PCH 的 I/O 解码范围也没有它，因此我猜测它管的是 PCH 的电源管理和 GPIO. 重点在后面的一个循环上::

  |           0x180000532      488b1de71c00.  mov rbx, qword [0x180002220] ; [0x180002220:8]=0
  |       ,=< 0x180000539      eb09           jmp 0x180000544
  |       |   ; CODE XREF from acpi_enable (0x180000547)
  |      .--> 0x18000053b      488bcb         mov rcx, rbx
  |      :|   0x18000053e      ff5308         call qword [rbx + 8]       ; r11
  |      :|   0x180000541      488b1b         mov rbx, qword [rbx]
  |      :|   ; CODE XREF from acpi_enable (0x180000539)
  |      :`-> 0x180000544      4885db         test rbx, rbx
  |      `==< 0x180000547      75f2           jne 0x18000053b

这是一个链表操作，链表结点里面有函数指针，这个循环调用了这些函数，奇怪的是，在这里 0x180002220 的内容是 NULL. 但是经过代码的修改，如果去掉了这个循环，EC ACPI 就无法正常使用，因此一定有别的地方填了这个链表。

通过 axt 0x180002220 查交叉引用，发现有两段代码引用了这个地址，对其中一段代码反汇编，可以猜测它所在函数的起始位置::

  |       :   0x1800006e0      3b08           cmp ecx, dword [rax]
  |      ,==< 0x1800006e2      7503           jne 0x1800006e7
  |      |:   0x1800006e4      4d8908         mov qword [r8], r9
  |      |:   ; CODE XREF from fcn.180000694 (0x1800006e2)
  |      `--> 0x1800006e7      488b01         mov rax, qword [rcx]
  |       :   0x1800006ea      498901         mov qword [r9], rax
  \       `=< 0x1800006ed      ebc6           jmp 0x1800006b5
              0x1800006ef      cc             int3
              0x1800006f0      48895c2408     mov qword [rsp + 8], rbx
              0x1800006f5      57             push rdi
              0x1800006f6      4883ec20       sub rsp, 0x20
              0x1800006fa      498bd8         mov rbx, r8
              0x1800006fd      488bfa         mov rdi, rdx
              0x180000700      4c8d05211b00.  lea r8, [0x180002228]
              0x180000707      488d15121b00.  lea rdx, [0x180002220]
              0x18000070e      b910000000     mov ecx, 0x10              ; 16
              0x180000713      e818ffffff     call fcn.180000630

在 int3 之后的指令应该是一个函数的开头，因此搜索是否有一个指向 0x1800006f0 的函数指针，果然搜出了结果::

  [0x1800003d8]> /x f006008001
  Searching 5 bytes in [0x180002460-0x1800024a0]
  hits: 0
  Searching 5 bytes in [0x180002320-0x180002460]
  hits: 0
  Searching 5 bytes in [0x180001800-0x180002320]
  hits: 1
  Searching 5 bytes in [0x180001680-0x180001800]
  hits: 0
  Searching 5 bytes in [0x1800002a0-0x180001680]
  hits: 0
  0x180001878 hit2_0 f006008001

查看 0x180001878 附近的内容，猜测 0x180001878 可能是某个协议/接口的实例，因此查一下交叉引用，果然找到一个函数用了它::

  [0x1800003d8]> axt 0x180001878
  fcn.180000ad8 0x180000c00 [DATA] lea r8, [0x180001878]

然后在 fcn.180000ad8 中找到::

  |  ||||||   0x180000bf9      488b05501600.  mov rax, qword [0x180002250]; gBS
  |  ||||||   0x180000c00      4c8d05710c00.  lea r8, [0x180001878]
  |  ||||||   0x180000c07      488d154a0c00.  lea rdx, [0x180001858]     ; GUID bd88ec68-...
  |  ||||||   0x180000c0e      488d4c2450     lea rcx, [var_50h]
  |  ||||||   0x180000c13      ff9048010000   call qword [rax + 0x148]   ; gBS->InstallMultipleProtocolInterfaces
  
在网上查了一下，以 bd88ec68 开头的 GUID 是 EFI_ACPI_EN_DISPATCH_PROTOCOL_GUID. 根据它对 acpi_enable 函数中使用的那个链表的引用，可以猜测其他模块可以用这个协议注册一些回调函数。在 UEFITool 中一搜索，发现其他的模块中只有 EcCommSmm 有这个 GUID，因此逆向这个模块，它果然用这个协议注册了一个回调函数，逆向这个回调函数，发现它往 EC 发送了一些命令，并设置了 GPI 作为 SCI 转发，把它实现在 coreboot 的 SMI 代码里面，刷入机器后，发现系统可以收到 ACPI 的 Qxx 查询，说明 EC ACPI 启用成功。
