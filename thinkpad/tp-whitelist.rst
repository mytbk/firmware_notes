再谈 ThinkPad 白名单
=====================

此前已经提过，删除 LenovoWmaPolicyDxe.efi 即可去除白名单，但是它的具体工作方式，我还是很好奇的。而且有的网友反映，部分机器直接删除这个模块会有些副作用，因此我来给它做个详细的分析。

在本文中，我用 X230 原厂固件的 LenovoWmaPolicyDxe.efi 模块。逆向工具仍然是 radare2,并配合我写的 `radare-uefi <https://github.com/mytbk/radare-uefi>`__ 脚本。

这个模块很简单，开头是几个 LocateProtocol，关键部分在最后::

  |    ||||   0x000110c7      488b442458     mov rax, qword [arg_58h]    ; [0x58:8]=-1 ; 'X' ; 88
  |    ||||   0x000110cc      4c8d0d050d00.  lea r9, [0x00011dd8]        ; protocol interface
  |    ||||   0x000110d3      488d15b6f3ff.  lea rdx, [0x00010490]
  |    ||||   0x000110da      488b08         mov rcx, qword [rax]
  |    ||||   0x000110dd      488364245000   and qword [arg_50h], 0
  |    ||||   0x000110e3      488d0532fdff.  lea rax, [0x00010e1c]
  |    ||||   0x000110ea      488905e70c00.  mov qword [0x00011dd8], rax ; [0x11dd8:8]=0
  |    ||||   0x000110f1      488b05b80c00.  mov rax, qword [0x00011db0] ; gBS ; [0x11db0:8]=0
  |    ||||   0x000110f8      48890dd10c00.  mov qword [0x00011dd0], rcx ; [0x11dd0:8]=0
  |    ||||   0x000110ff      488d4c2450     lea rcx, [arg_50h]          ; 0x50 ; 'P' ; 80
  |    ||||   0x00011104      4533c0         xor r8d, r8d
  |    ||||   0x00011107      ff9080000000   call qword [rax + 0x80]     ; 128 ; "gBS->InstallProtocolInterface"
  |    ||||   0x0001110d      4c8b5c2450     mov r11, qword [arg_50h]    ; [0x50:8]=-1 ; 'P' ; 80
  |    ||||   0x00011112      4533c0         xor r8d, r8d
  |    ||||   0x00011115      418d4810       lea ecx, [r8 + 0x10]        ; 16
  |    ||||   0x00011119      4c8d4c2420     lea r9, [local_20h]         ; 0x20 ; 32
  |    ||||   0x0001111e      488d15affeff.  lea rdx, [0x00010fd4]
  |    ||||   0x00011125      4c891d6c0c00.  mov qword [0x00011d98], r11 ; [0x11d98:8]=0
  |    ||||   0x0001112c      e83f080000     call fcn.00011970
  
有两个地方非常可疑，一个是 InstallProtocolInterface，它安装一个协议，用于被其他模块调用；另一个是 fcn.00011970，我们先看这个::

  / (fcn) fcn.00011970 67
  |   fcn.00011970 (int arg6, int arg5, int arg3, int arg4);
  |           ; var int local_20h @ rsp+0x20
  |           ; var int local_28h @ rsp+0x28
  |           ; arg int arg6 @ r9
  |           ; arg int arg5 @ r8
  |           ; arg int arg3 @ rdx
  |           ; arg int arg4 @ rcx
  |           ; CALL XREF from fcn.69588 (0x1112c)
  |           0x00011970      4883ec38       sub rsp, 0x38               ; '8'
  |           0x00011974      4885d2         test rdx, rdx
  |           0x00011977      4c894c2428     mov qword [local_28h], r9   ; arg6
  |           0x0001197c      488d05e9ffff.  lea rax, [0x0001196c]
  |           0x00011983      480f44d0       cmove rdx, rax
  |           0x00011987      488d05f2eaff.  lea rax, [0x00010480]       ; gEfiEventReadyToBootGuid
  |           0x0001198e      4d8bc8         mov r9, r8                  ; arg5
  |           0x00011991      4889442420     mov qword [local_20h], rax
  |           0x00011996      488b05130400.  mov rax, qword [0x00011db0] ; gBS ; [0x11db0:8]=0
  |           0x0001199d      4c8bc2         mov r8, rdx                 ; arg3
  |           0x000119a0      488bd1         mov rdx, rcx                ; arg4
  |           0x000119a3      b900020000     mov ecx, 0x200              ; 512
  |           0x000119a8      ff9070010000   call qword [rax + 0x170]    ; 368 ; "gBS->CreateEventEx"
  |           0x000119ae      4883c438       add rsp, 0x38               ; '8'
  
它创建了一个事件，并且用了 gEfiEventReadyToBootGuid，说明这是在加载 bootloader 前要调一个函数。事实上，通过搜索 EDK2 的源码，可以在 EdkCompatibilityPkg/Foundation/Library/RuntimeDxe/EfiRuntimeLib/Event.c 找到这个函数，函数名叫 RtEfiCreateEventReadyToBoot. 我们看它的第2个参数 NotifyFunction，也就是 edx 的值 0x00010fd4. 先执行命令 ``af @ fcn.00010fd4`` 让 radare2 分析这个函数，再 ``pdf @ fcn.00010fd4``.

这里省略此函数的代码。看这个函数，它是用了此前 LocateProtocol 找到的一个协议的实例，调用了它的两个函数，似乎没什么和白名单检测相关的地方。我们接下来看看用 InstallProtocolInterface 安装的那个协议。

首先看协议的 GUID::

  [0x0001102c]> x 16 @ 0x00010490
  - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D  E F  0123456789ABCDEF
  0x00010490  d4a9 1242 924a fb4b b47f 2734 355f 54ac  ...B.J.K..'45_T.
  
用 UEFITool 找 GUID 4212a9d4-...，结果发现，只有3个模块引用了这个 GUID，除了 LenovoWmaPolicyDxe.efi 外，就是 LenovoWmaPciDxe.efi 和 LenovoWmaUsbDxe.efi，看名字就知道，它们是用于检测 PCI 和 USB 设备的白名单的。

接着看 protocol interface 0x00011dd8，注意后面跟着的指令，发现程序往这个地方写了一个值 0x00010e1c. 尝试用 af 分析该函数再用 pdf 打印，发现它的确是一个函数。当然了，也可以通过 LenovoWmaPciDxe.efi 看这个协议的使用方式，可以知道往 protocol interface 里面填的就是个函数指针。

接下来就是分析 0x00010e1c，通过看汇编代码，发现里面的转移指令都是向后转移，不存在循环。而白名单的检测应该是使用循环对可用的设备 ID 进行逐个检测。因此我们看这个函数里面调用的一个函数 fcn.00010b20，里面有循环代码::

  |    | ||   ; CODE XREFS from fcn.00010b20 (0x10c25, 0x10c69, 0x10c7b, 0x10ccd)
  | ...-.---> 0x00010bdd      488bcf         mov rcx, rdi
  | :::|:||   0x00010be0      e827feffff     call fcn.00010a0c
  | :::|:||   0x00010be5      4885c0         test rax, rax
  | ========< 0x00010be8      7913           jns 0x10bfd
  | :::|:||   0x00010bea      4489ac24a800.  mov dword [arg_a8h], r13d
  | :::|:||   ; CODE XREF from fcn.00010b20 (0x10bfb)
  | --------> 0x00010bf2      8b8424a80000.  mov eax, dword [arg_a8h]    ; [0xa8:4]=-1 ; 168
  | :::|:||   0x00010bf9      85c0           test eax, eax
  | ========< 0x00010bfb      75f5           jne 0x10bf2
  | :::|:||   ; CODE XREF from fcn.00010b20 (0x10be8)
  | --------> 0x00010bfd      48b807000000.  movabs rax, 0x8000000000000007
  | :::|:||   ; CODE XREFS from fcn.00010b20 (0x10b4a, 0x10e07, 0x10e17)
  | ---`----> 0x00010c07      488b9c249000.  mov rbx, qword [arg_90h]    ; [0x90:8]=-1 ; 144
  | ::: :||   0x00010c0f      4883c470       add rsp, 0x70               ; 'p'
  | ::: :||   0x00010c13      415d           pop r13
  | ::: :||   0x00010c15      5f             pop rdi
  | ::: :||   0x00010c16      5e             pop rsi
  | ::: :||   0x00010c17      c3             ret
  | ::: :||   ; CODE XREF from fcn.00010b20 (0x10b56)
  | ::: :`--> 0x00010c18      8b0552f6ffff   mov eax, dword [0x00010270] ; [0x10270:4]=0
  | ::: : |   0x00010c1e      664533c9       xor r9w, r9w
  | ::: : |   0x00010c22      83f806         cmp eax, 6                  ; 6
  | `=======< 0x00010c25      74b6           je 0x10bdd
  |  :: : |   0x00010c27      4533c0         xor r8d, r8d
  |  :: : |   0x00010c2a      488d153ff6ff.  lea rdx, [0x00010270]
  |  :: : |   ; CODE XREF from fcn.00010b20 (0x10c67)
  |  :: :.--> 0x00010c31      413bc5         cmp eax, r13d
  |  ::,====< 0x00010c34      7405           je 0x10c3b
  |  ::|::|   0x00010c36      83f805         cmp eax, 5                  ; 5
  | ,=======< 0x00010c39      7519           jne 0x10c54
  | |::|::|   ; CODE XREF from fcn.00010b20 (0x10c34)
  | |::`----> 0x00010c3b      410fb74c1006   movzx ecx, word [r8 + rdx + 6] ; [0x6:2]=0xffff ; 6
  | |:: ::|   0x00010c41      410fb7441004   movzx eax, word [r8 + rdx + 4] ; [0x4:2]=0xffff ; 4
  | |:: ::|   0x00010c47      c1e110         shl ecx, 0x10
  | |:: ::|   0x00010c4a      0bc8           or ecx, eax
  | |:: ::|   0x00010c4c      390f           cmp dword [rdi], ecx        ; [0x16:4]=-1 ; 22
  | |::,====< 0x00010c4e      0f849f010000   je 0x10df3
  | |::|::|   ; CODE XREF from fcn.00010b20 (0x10c39)
  | `-------> 0x00010c54      664503cd       add r9w, r13w
  |  ::|::|   0x00010c58      450fb7c1       movzx r8d, r9w
  |  ::|::|   0x00010c5c      49c1e004       shl r8, 4
  |  ::|::|   0x00010c60      418b0410       mov eax, dword [r8 + rdx]
  |  ::|::|   0x00010c64      83f806         cmp eax, 6                  ; 6
  |  ::|:`==< 0x00010c67      75c8           jne 0x10c31
  |  `======< 0x00010c69      e96fffffff     jmp 0x10bdd
  |   :|: |   ; CODE XREF from fcn.00010b20 (0x10b38)
  |   :|: `-> 0x00010c6e      8b05fcf5ffff   mov eax, dword [0x00010270] ; [0x10270:4]=0
  |   :|:     0x00010c74      664533c9       xor r9w, r9w
  |   :|:     0x00010c78      83f806         cmp eax, 6                  ; 6
  |   `=====< 0x00010c7b      0f845cffffff   je 0x10bdd
  |    |:     0x00010c81      4533c0         xor r8d, r8d
  |    |:     0x00010c84      488d15e5f5ff.  lea rdx, [0x00010270]
  |    |:     ; CODE XREF from fcn.00010b20 (0x10cd3)
  |    |: .-> 0x00010c8b      85c0           test eax, eax
  |    |:,==< 0x00010c8d      752b           jne 0x10cba
  |    |:|:   0x00010c8f      410fb74c1006   movzx ecx, word [r8 + rdx + 6] ; [0x6:2]=0xffff ; 6
  |    |:|:   0x00010c95      410fb7441004   movzx eax, word [r8 + rdx + 4] ; [0x4:2]=0xffff ; 4
  |    |:|:   0x00010c9b      c1e110         shl ecx, 0x10
  |    |:|:   0x00010c9e      0bc8           or ecx, eax
  |    |:|:   0x00010ca0      390f           cmp dword [rdi], ecx        ; [0x16:4]=-1 ; 22
  |   ,=====< 0x00010ca2      7516           jne 0x10cba
  |   ||:|:   0x00010ca4      410fb74c100a   movzx ecx, word [r8 + rdx + 0xa] ; [0xa:2]=0xffff ; 10
  |   ||:|:   0x00010caa      410fb7441008   movzx eax, word [r8 + rdx + 8] ; [0x8:2]=0xffff ; 8
  |   ||:|:   0x00010cb0      c1e110         shl ecx, 0x10
  |   ||:|:   0x00010cb3      0bc8           or ecx, eax
  |   ||:|:   0x00010cb5      394f04         cmp dword [rdi + 4], ecx    ; [0x16:4]=-1 ; 22
  |  ,======< 0x00010cb8      741b           je 0x10cd5
  |  |||:|:   ; CODE XREFS from fcn.00010b20 (0x10c8d, 0x10ca2)
  |  |`--`--> 0x00010cba      664503cd       add r9w, r13w
  |  | |: :   0x00010cbe      450fb7c1       movzx r8d, r9w
  |  | |: :   0x00010cc2      49c1e004       shl r8, 4
  |  | |: :   0x00010cc6      418b0410       mov eax, dword [r8 + rdx]
  |  | |: :   0x00010cca      83f806         cmp eax, 6                  ; 6
  |  | |`===< 0x00010ccd      0f840affffff   je 0x10bdd
  |  | |  `=< 0x00010cd3      ebb6           jmp 0x10c8b
  
里面调用了一个函数 fcn.00010a0c，查看这个函数，可以发现里面有些格式字符串 ``u"%04x/%04x"`` ，可以猜测这是检测到设备不在白名单时输出信息用的。再看看循环里面的寻址 ``movzx ecx, word [r8 + rdx + 6]`` ，前面 r8 被清零，rdx 设为一个地址 0x00010270，要访问的就在这个地址之后，我们 ``x @ 0x00010270`` 可以看到，里面的确是一堆含有设备 ID 的数据。

于是要去除白名单检测，有几种方法：

1. 访问模块内设备 ID 白名单之后有 cmp 比较指令，之后是个条件转移，可以把 je 改为无条件跳转 jmp
2. 在循环结束后，如果检测到设备不在白名单之中，就跳往 0x10bdd，之后就是输出错误信息，我们可以把这条指令改为跳到循环之后的地址 0x10cd5

鉴于我现在没有机器测试，我的分析暂时就到这里。
