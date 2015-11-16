## 在CubieTruck上使用EHCI Gadget Debug成功
感谢Hans De Goede等人，他们把Allwinner芯片的OTG驱动成功移植到了主线内核，使得我可以在CubieTruck上使用更稳定的主线内核来做我的工作。

要在CubieTruck上使用EHCI Gadget Debug的功能，需要编译出`g_dbgp`模块，更重要的是打开OTG功能。对于Allwinner的开发板，打开OTG功能具体要选上的选项有:
```
Device Drivers
--- USB Drivers
[*] OTG Support
<*> Inventra Highspeed Dual Role Controller (TI, ADI, AW, ...)
    MUSB Mode Selection (Dual Role mode)  --->
    *** Platform Glue Layer ***
    <*>     Allwinner (sunxi)
    *** MUSB DMA mode ***
    [*]     Disable DMA (always use PIO)
    --- USB Physical Layer drivers
    <*> NOP USB Transceiver Driver
```

（鉴于现在Allwinner的DMA还没merge进主线内核，因此需要选上Disable DMA的选项。）

进入系统后modprobe g_dbgp就行了，然后查看`/dev/ttyGS0`就能看到被调试机器上的输出。本人已在Lenovo ThinkPad X60上成功看到了Libreboot的输出。
