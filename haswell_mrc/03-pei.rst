mrc.bin 中的 PEI 运行时
==================================================

时间：2018.12.22

又做了十多天的逆向，我现在对这几天的工作做点总结。

对一些函数做了替换后，现在的我改造后的 Haswell 平台初始化代码分为几个部分：

- mrc.asm
- 其他的汇编程序
- 逆向后重写的 C 程序

经过代码重写和分析，可以看出，mrc.bin 的确有一套 PEI 运行时，不过只实现了部分 API. 现在已经重写为 C 代码。具体可见 [1]_.

.. [1] https://github.com/mytbk/coreboot/blob/haswell-dumpbin/src/northbridge/intel/haswell/pei_svc.c
