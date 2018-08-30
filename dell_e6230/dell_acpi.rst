Dell Latitude E6230 ACPI 支持
=============================

操作系统启用 ACPI 支持的流程是，从 FADT 中找到 SMI 端口和命令，向 SMI 端口写入启用 ACPI 的命令，这个对于几乎所有 x86 的机器是一样的。

而 Dell Latitude E6230 在这个 SMI 被触发后，往 SMI 端口 0xb8 写入了 0xa0，也就是说，会触发另一个 SMI.

为了找到 SMI 命令 0xa0 的处理程序，我们需要找 gEfiSmmSwDispatchProtocolGuid，于是最终可以找到几个相关的 EFI 模块。最后，在 RuntimeAcpiSmm 中找到了如下代码（相关函数名是我做的标记）::

  [0x1800002e8]> pdf @ acpi_enable_disable
  / (fcn) acpi_enable_disable 99
  |   acpi_enable_disable (int arg_30h, int arg_38h);
  |           ; arg int arg_30h @ rsp+0x30
  |           ; arg int arg_38h @ rsp+0x38
  |           ; CALL XREF from entry0 (0x180000372)
  |           0x180000478      4883ec28       sub rsp, 0x28              ; '('
  |           0x18000047c      0fb6056a0c00.  movzx eax, byte [0x1800010ed] ; [0x1800010ed:1]=0
  |           0x180000483      4c8d4c2438     lea r9, [arg_38h]          ; 0x38 ; '8' ; 56
  |           0x180000488      4c8d442430     lea r8, [arg_30h]          ; 0x30 ; '0' ; 48
  |           0x18000048d      4889442430     mov qword [arg_30h], rax
  |           0x180000492      488b055f0c00.  mov rax, qword [0x1800010f8] ; SmmSwDispatch ; [0x1800010f8:8]=0
  |           0x180000499      488d154cffff.  lea rdx, [acpi_enable]     ; 0x1800003ec ; "H\x83\xec(\x80=E\t"
  |           0x1800004a0      488bc8         mov rcx, rax
  |           0x1800004a3      ff10           call qword [rax]
  |           0x1800004a5      488b054c0c00.  mov rax, qword [0x1800010f8] ; SmmSwDispatch ; [0x1800010f8:8]=0
  |           0x1800004ac      440fb61d3a0c.  movzx r11d, byte [0x1800010ee] ; [0x1800010ee:1]=0
  |           0x1800004b4      4c8d4c2438     lea r9, [arg_38h]          ; 0x38 ; '8' ; 56
  |           0x1800004b9      4c8d442430     lea r8, [arg_30h]          ; 0x30 ; '0' ; 48
  |           0x1800004be      488d1577ffff.  lea rdx, [0x18000043c]
  |           0x1800004c5      488bc8         mov rcx, rax
  |           0x1800004c8      4c895c2430     mov qword [arg_30h], r11
  |           0x1800004cd      ff10           call qword [rax]
  |           0x1800004cf      c60566080000.  mov byte [0x180000d3c], 0  ; [0x180000d3c:1]=0
  |           0x1800004d6      4883c428       add rsp, 0x28              ; '('
  \           0x1800004da      c3             ret
  
注意到 0x1800010ed 和 0x1800010ee 处两个字节在这个模块中的初值均为 0. 一般来说，SMM Sw Dispatch Protocol 是不应该注册一个 SMI 命令为 0 的处理程序的，只能理解为这个模块在这之前写了这两个字节。

我们用 radare2 的 axt 命令找交叉引用， ``axt 0x1800010ed`` 找不到其他的引用，我尝试在 16 字节的边界找，用 ``axt 0x1800010e0`` 命令，果然找到了交叉引用。在入口函数处有::

  |     |`--> 0x180000324      488d15b50d00.  lea rdx, [0x1800010e0]
  |     | |   0x18000032b      488d0dfe0a00.  lea rcx, [0x180000e30]
  |     | |   0x180000332      41b818000000   mov r8d, 0x18              ; 24
  |     | |   0x180000338      e863ffffff     call fcn.1800002a0
  
跟进 fcn.180002a0 可以找到::

  / (fcn) fcn.1800002a0 70
  |   fcn.1800002a0 (int arg5, int arg3, int arg_8h, int arg_30h, int arg_48h);
  |           ; arg int arg_8h @ rsp+0x8
  |           ; arg int arg_30h @ rsp+0x30
  |           ; arg int arg_48h @ rsp+0x48
  |           ; arg int arg5 @ sp
  |           ; arg int arg3 @ sf
  |           ; CALL XREF from entry0 (0x180000338)
  |           0x1800002a0      48895c2408     mov qword [arg_8h], rbx    ; [00] -r-x section size 2560 named .text
  |           0x1800002a5      57             push rdi
  |           0x1800002a6      4883ec20       sub rsp, 0x20
  |           0x1800002aa      488b05ff0e00.  mov rax, qword [0x1800011b0] ; gBS ; [0x1800011b0:8]=0
  |           0x1800002b1      498bd8         mov rbx, r8
  |           0x1800002b4      488bfa         mov rdi, rdx
  |           0x1800002b7      4c8d442448     lea r8, [arg_48h]          ; 0x48 ; 'H' ; 72
  |           0x1800002bc      33d2           xor edx, edx
  |           0x1800002be      ff9040010000   call qword [rax + 0x140]   ; 320 ; "gBS->LocateProtocol"
  |           0x1800002c4      4885c0         test rax, rax
  |       ,=< 0x1800002c7      7812           js 0x1800002db
  |       |   0x1800002c9      488b542448     mov rdx, qword [arg_48h]   ; [0x48:8]=-1 ; 'H' ; 72
  |       |   0x1800002ce      4c8bc3         mov r8, rbx
  |       |   0x1800002d1      488bcf         mov rcx, rdi
  |       |   0x1800002d4      e867090000     call memmove               ; void *memmove(void *s1, const void *s2, size_t n)
  |       |   0x1800002d9      33c0           xor eax, eax
  |       |   ; CODE XREF from fcn.1800002a0 (0x1800002c7)
  |       `-> 0x1800002db      488b5c2430     mov rbx, qword [arg_30h]   ; [0x30:8]=-1 ; '0' ; 48
  |           0x1800002e0      4883c420       add rsp, 0x20
  |           0x1800002e4      5f             pop rdi
  \           0x1800002e5      c3             ret
  
于是就能看得很明白了，程序先用 LocateProtocol 找一个协议（协议的 GUID 的地址在 rcx 中），LocateProtocol 把这个协议的接口放在 r8 指向的地址。接着是一个 memmove 把这个协议的信息都复制到上文提到的 0x1800010e0 处。

那么这个协议是什么呢，看一下 0x180000e30 可以看到::

  0x180000e30  4aab e6e5 7d0d 064a a164 61e4 7cce 9b3b

用 UEFITool 找这个 GUID 可以找到一个叫 DellAcpiConfig 的模块，它就一个功能，用 InstallProtocolInterface 安装这个协议，那么这个 Interface 的内容是什么呢？根据 InstallProtocolInterface 的参数 r9 可以看到这个协议的 Interface::

  0x180000440  0100 0000 0000 0000 b200 0e03 04a0 a100  ................
  0x180000450  0000 0000 0000 0000 28b8 b2d2 2608 a748  ........(...&..H

可以看到，在偏移为 0xd 和 0xe 的位置分别是 0xa0 和 0xa1，这就是启用和禁用 ACPI 的 SMI 命令，于是我们就找到了固件处理 SMI 命令 0xa0 的函数了。
