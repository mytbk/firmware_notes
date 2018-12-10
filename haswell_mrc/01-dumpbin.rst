r2dumpbin - 将 mrc.bin 转为 NASM 汇编源程序
==================================================

时间：2018.12.11

此前在做一些逆向工作的时候，有过一个想法，就是先把程序的汇编提取出来，如果汇编后程序可以使用，就说明这段汇编能独立工作，并且能链接到已有的程序。之后再把这些汇编源码逐步转成 C 代码，这样就完成了逆向。一个实际的例子是逆向大富翁4的 mkf 文件解压函数，当时发现我逆向后重写的代码逻辑有问题，于是我提取出汇编，再逐步重写成 C，最终得到能工作的代码。[1]_

coreboot 的 Haswell 和 Broadwell 内存控制器初始化的代码使用一个叫 mrc.bin 的 blob. 和 FSP 不同，它是一个平坦的二进制文件，文件开头即为代码，并且是程序的入口。从 coreboot 源码可以看到，mrc.bin 需要放在一个固定的位置，在内存中的地址是 0xfffa0000. [2]_ 同时，这个文件不分段，因此有可能代码和数据会以任意形式混合存放。

在 2018.12.05 这天，我开始编写 dumpbin，它是在逆向工程平台 `radare2 <https://radare.org>`__ 上运行的 r2pipe 脚本。后来考虑到这个名字已经被其他项目使用，于是改名为 `r2dumpbin <https://github.com/mytbk/r2dumpbin>`__. 经过将近一星期的工作，终于完成了这个脚本，它具有以下特性：

- 将 mrc.bin 转为汇编源程序，它可以用 `NASM <https://www.nasm.us/>`__ 汇编，生成和原来的 mrc.bin 完全相同的二进制文件
- 分析出所有的函数和数据引用，所有可能的指针都转为汇编中的标签
- 找出 mrc.bin 中的 ASCII 字符串

今天我尝试分析了一下生成的汇编，有以下发现：

- 在程序入口处，初始化栈顶为 0xff800000. 此外设置了一个栈底，放了几个全局变量。
- 程序有一个 printf 实现。
- 里面存在几个 GUID，可以在网上搜索到，怀疑里面实现了一套 UEFI PEI API.

.. [1] 可参见 `此提交 <https://git.wehack.space/rich4/commit/?id=96c300d7755ce73c34d75d22faefcdd418fd5a46>`__
.. [2] 参见 `nb/intel/haswell/Makefile.inc 36行 <https://review.coreboot.org/cgit/coreboot.git/tree/src/northbridge/intel/haswell/Makefile.inc#n36>`__
