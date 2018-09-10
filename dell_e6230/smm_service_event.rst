SMM Service 和 SMM Event 协议
===============================

之前找到了 SMM Runtime 协议，然后发现 ``acpi_enable`` 函数调用了一些协议里的一个函数，这个协议的 GUID 为::

 0x180000ee0  8065 1f0c 579c 324a 8499 227c 2ea2 7e7d

之后我花了很长时间，在 UEFITool 里根据这个 GUID 找到了所有可能安装了这个协议的模块，最终在 SmmServices 中找到了如下代码::

 |           0x180000280      4053           push rbx                   ; [00] -r-x section size 832 named .text
 |           0x180000282      4883ec20       sub rsp, 0x20
 |           0x180000286      4c8d442440     lea r8, [arg_40h]          ; 0x40 ; '@' ; 64
 |           0x18000028b      33db           xor ebx, ebx
 |           0x18000028d      e86a010000     call fcn.1800003fc
 |           0x180000292      385c2440       cmp byte [arg_40h], bl     ; [0x5:1]=255 ; 5
 |       ,=< 0x180000296      7467           je 0x1800002ff
 |       |   0x180000298      448d4330       lea r8d, [rbx + 0x30]      ; '0' ; 48
 |       |   0x18000029c      488d0d7d0400.  lea rcx, [0x180000720]
 |       |   0x1800002a3      33d2           xor edx, edx
 |       |   0x1800002a5      e8d6020000     call memset                ; void *memset(void *s, int c, size_t n)
 |       |   0x1800002aa      488b05af0400.  mov rax, qword [0x180000760] ; gBS ; [0x180000760:8]=0
 |       |   0x1800002b1      4c8d0d680400.  lea r9, [0x180000720]      ; protocol interface
 |       |   0x1800002b8      488d15010400.  lea rdx, [0x1800006c0]
 |       |   0x1800002bf      488d4c2448     lea rcx, [arg_48h]         ; 0x48 ; 'H' ; 72
 |       |   0x1800002c4      4533c0         xor r8d, r8d
 |       |   0x1800002c7      48895c2448     mov qword [arg_48h], rbx
 |       |   0x1800002cc      ff9080000000   call qword [rax + 0x80]    ; 128 ; "gBS->InstallProtocolInterface"
 |       |   0x1800002d2      4885c0         test rax, rax
 |      ,==< 0x1800002d5      782b           js 0x180000302
 |      ||   0x1800002d7      488b05120400.  mov rax, qword [0x1800006f0] ; [0x1800006f0:8]=0
 |      ||   0x1800002de      4c8d0d3b0400.  lea r9, [0x180000720]
 |      ||   0x1800002e5      488d15d40300.  lea rdx, [0x1800006c0]
 |      ||   0x1800002ec      488d4c2448     lea rcx, [arg_48h]         ; 0x48 ; 'H' ; 72
 |      ||   0x1800002f1      4533c0         xor r8d, r8d
 |      ||   0x1800002f4      48895c2448     mov qword [arg_48h], rbx
 |      ||   0x1800002f9      ff5008         call qword [rax + 8]       ; 8

其中 0x1800006c0 里面存的就是上面说的那个 GUID，SmmServices 用 boot service 和 SMM Runtime Protocol 的 InstallProtocolInterface 分别在 UEFI 的 DXE 和 SMM 中都安装了这个协议。但是注意到上面的 memset，它将这个协议的接口中的内容都置为 0. 那么这个协议到底怎么用呢？

我也尝试找过其他的模块，但是它们都没有安装这个协议。直到后来逆向了一个模块才找到了灵感。在 RuntimeAcpiSmm 里面有::

 |   |||||   0x1800003d0      488b05510b00.  mov rax, qword [0x180000f28] ; [0x180000f28:8]=0
 |   |||||   0x1800003d7      48897020       mov qword [rax + 0x20], rsi

其中 0x180000f28 就是用 SMM Runtime Protocol 的 LocateProtocol 找到的一个这个 GUID 的协议的实例，接下来往这个协议的接口里面写了东西。这时我才意识到，SmmServices 安装了这个协议，但是协议的接口只是用全 0 的数据初始化，而其他的 UEFI 模块则定位出这个协议，往这个协议的接口里面填入信息。

之后我重新看 DellNbEcSmm （这是我之前找到的一个和 ACPI 相关的关键模块）时，发现了这些东西::

 |           0x180002df4      33db           xor ebx, ebx
 |           0x180002df6      e8d54a0000     call sub.gST______void____0_8d0
 |           0x180002dfb      385c2460       cmp byte [arg_60h], bl     ; [0x5:1]=255 ; 5
 |       ,=< 0x180002dff      0f8400020000   je 0x180003005
 |       |   0x180002e05      48391d6c7f00.  cmp qword [0x18000ad78], rbx ; [0x25:8]=-1 ; '%' ; 37
 |       |   0x180002e0c      4c8d251d5c00.  lea r12, str.DellNbEcSmm.c ; 0x180008a30 ; "DellNbEcSmm.c"
 |      ,==< 0x180002e13      7514           jne 0x180002e29
 |      ||   0x180002e15      4c8d058c5c00.  lea r8, str.gSmmRt______void____0 ; 0x180008aa8 ; "gSmmRt != ((void *) 0)"
 |      ||   0x180002e1c      ba68010000     mov edx, 0x168             ; 360
 |      ||   0x180002e21      498bcc         mov rcx, r12
 |      ||   0x180002e24      e8f7480000     call fcn.180007720
 |      ||   ; CODE XREF from entry0 (0x180002e13)
 |      `--> 0x180002e29      488b05507f00.  mov rax, qword [0x18000ad80] ; gSmmServ ; [0x18000ad80:8]=0
 |       |   0x180002e30      4885c0         test rax, rax
 |      ,==< 0x180002e33      751b           jne 0x180002e50
 |      ||   0x180002e35      4c8d054c5c00.  lea r8, str.gSmmServ______void____0 ; 0x180008a88 ; "gSmmServ != ((void *) 0)"
 |      ||   0x180002e3c      ba69010000     mov edx, 0x169             ; 361
 |      ||   0x180002e41      498bcc         mov rcx, r12
 |      ||   0x180002e44      e8d7480000     call fcn.180007720
 |      ||   0x180002e49      488b05307f00.  mov rax, qword [0x18000ad80] ; gSmmServ ; [0x18000ad80:8]=0
 |      ||   ; CODE XREF from entry0 (0x180002e33)
 |      `--> 0x180002e50      48395810       cmp qword [rax + 0x10], rbx ; [0x25:8]=-1 ; '%' ; 37
 |      ,==< 0x180002e54      751b           jne 0x180002e71
 |      ||   0x180002e56      4c8d05035c00.  lea r8, str.gSmmServ__SmmEvent______void____0 ; 0x180008a60 ; "gSmmServ->SmmEvent != ((void *) 0)"
 |      ||   0x180002e5d      ba6a010000     mov edx, 0x16a             ; 362
 |      ||   0x180002e62      498bcc         mov rcx, r12
 |      ||   0x180002e65      e8b6480000     call fcn.180007720
 |      ||   0x180002e6a      488b050f7f00.  mov rax, qword [0x18000ad80] ; gSmmServ ; [0x18000ad80:8]=0
 |      ||   ; CODE XREF from entry0 (0x180002e54)
 |      `--> 0x180002e71      483918         cmp qword [rax], rbx       ; [0x25:8]=-1 ; '%' ; 37
 |      ,==< 0x180002e74      751b           jne 0x180002e91
 |      ||   0x180002e76      4c8d05c35b00.  lea r8, str.gSmmServ__Delay______void____0 ; 0x180008a40 ; "gSmmServ->Delay != ((void *) 0)"
 |      ||   0x180002e7d      ba6b010000     mov edx, 0x16b             ; 363
 |      ||   0x180002e82      498bcc         mov rcx, r12
 |      ||   0x180002e85      e896480000     call fcn.180007720
 
注意到里面的一些字符串。这个模块不知道什么原因，在构建的时候并没有把那些 assert 类的语句删掉，从而留下了一些线索。我们已经知道了那个 GUID 来自 SmmServices，而从反汇编的分析中也可以验证它对应于这个模块里的 gSmmServ. 接下来看我们关注的 gSmmServ+0x10 的位置，因为 ``acpi_enable`` 中访问了这个位置的接口的一个函数。因此我们往下看，就发现这是 gSmmServ->SmmEvent. 因此我们接下来就应该看 SmmEvent 这个模块。果然，在 SmmEvent 里面能看到::

 |       |   0x18000099d      488b05ac0700.  mov rax, qword [0x180001150] ; [0x180001150:8]=0
 |       |   0x1800009a4      488d35450600.  lea rsi, [0x180000ff0]
 |       |   0x1800009ab      488d153e0700.  lea rdx, [0x1800010f0]
 |       |   0x1800009b2      4c8bce         mov r9, rsi                ; protocol interface
 |       |   0x1800009b5      ff5008         call qword [rax + 8]       ; 8
 |       |   0x1800009b8      488bd8         mov rbx, rax
 |       |   0x1800009bb      4885c0         test rax, rax
 |      ,==< 0x1800009be      7834           js 0x1800009f4
 |      ||   0x1800009c0      488b05910700.  mov rax, qword [0x180001158] ; [0x180001158:8]=0
 |      ||   0x1800009c7      48897010       mov qword [rax + 0x10], rsi
 
先是用 SMM Runtime Protocol 的 InstallProtocolInterface 安装了这个协议，然后还把协议的接口地址写进了 gSmmServ->SmmEvent 的位置，因此，接下来的任务就是看 SmmEvent 的接口，即地址 0x180000ff0 处的数据。尤其是要关注在 0x180000ff0+0x10 处存放的函数指针，因为 ``acpi_enable`` 调用的就是这个函数。
