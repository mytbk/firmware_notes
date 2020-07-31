在coreboot上使用Crystal Well处理器的一点记录
=========================================================

自从coreboot移植到了ASRock H81M-HDS之后，我开始对Haswell的机器开始感兴趣，我还自己写了Haswell的autoport并把coreboot移植到了Lenovo ThinkPad T440p.有不少网友在T440p上使用coreboot.有些T440p用户提出了一个问题，Crystal Well的CPU在用原厂固件的T440p上能用，但是在coreboot中不可用。

几个月前我看了coreboot邮件列表上关于CRW的问题，里面有一份coreboot启动时的日志，我发现CRW的Host Bridge的PCI ID和已知的Haswell CPU的Host Bridge不一样。当初我在H81M-HDS上安装E3处理器也遇到了同样的问题。解决方法很简单，就是在 nb/intel/haswell/northbridge.c 中加上对应的PCI ID就行了。

由于我没有硬件测试，因此那个patch一直放着。前几天我终于入手了一块CRW的CPU，是一块魔改的i5-4570R.装上H81M-HDS，在原厂固件下可以点亮机器，说明CPU没问题，我可以开始试了。

结果刷上coreboot之后，连上串口，发现开机之后没有输出，但是把CPU换成奔腾之后就有串口输出。因此我需要先排查这个问题。

我尝试使用SerialICE，发现SerialICE串口终端可以用。也就是说这块CPU并没有影响串口的功能。针对这点，我怀疑问题有可能是以下两点之一：

- coreboot在初始化串口之前出了问题
- CAR出了问题

我之所以会怀疑CAR，是因为CRW处理器中多了一级eDRAM存储层次，而SerialICE偏偏就没有CAR，而是使用ROMCC.而且我用SerialICE调coreboot的时候，发现就在CAR代码那里就卡住了，事后回顾，我觉得应该是SerialICE出错了。

关于CAR和ROMCC，我想起了coreboot去年有一个 ``C_ENVIRONMENT_BOOTBLOCK`` 的架构重构，它是把CAR提前到更早的阶段，并废除ROMCC.我试着使用commit c4772b9fd，它之后的提交就是使用 ``C_ENVIRONMENT_BOOTBLOCK``.结果发现用这个版本就有串口输出了。

既然找到了一个能用的版本，我干脆就直接做bisect.结果发现第一个导致串口无输出的版本是commit 45ddb434.它和CAR完全无关，而是一个记录printk控制台输出时间的patch.

为了确认是记录时间相关的代码造成的问题，我把 console/printk.c 里的 ``TRACK_CONSOLE_TIME`` 宏设成0,结果就有串口输出了，于是可以确认问题和记录时间用的 monotonic timer 相关的代码有关。接着我查找 x86 中 ``timer_monotonic_get()`` 调用的相关代码，终于发现了 cpu/intel/common/fsb.c 中 ``get_fsb_tsc()`` 和 CPUID 相关，加上 CRW 的 ID 之后就有串口输出了。

最后再把之前发现缺少的 CPU 和北桥 PCI 设备的 ID 补上，编译固件并启动，看到了正常的串口输出，并能点亮机器进入操作系统。Crystal Well 的 coreboot 支持就做完了。

总结一下，之前我考虑过可能是串口初始化之前出了问题，或者是CAR出了问题。实际上已有的这些代码都没问题。真正出问题的是计时需要的 monotonic timer 相关的代码，因为缺少了 CRW 的 CPUID，导致无法获得 FSB 频率的数值（实际上FSB几代之前早就没了），计时相关的代码出错，最终 printk 功能异常。发现这个问题的最终方式是通过 bisect 定位问题，最终追踪到导致问题的代码。顺便提一下，bisect的时候，可以用 ``make BUILD_TIMELESS=1`` 构建 coreboot，它可以做 coreboot 的可重现构建，只要用到的代码不变，最终生成的 ROM 不会变，于是在 bisect 后期多个版本会产生相同的 ROM，这样可以加快 bisect 的速度。
