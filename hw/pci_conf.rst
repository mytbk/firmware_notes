PCI配置空间的访问
=================

PCI和PCI Express设备都可以对设备的PCI地址的读写来访问。PCI地址有3部分:bus,device,function,简写为bdf.用bdf和配置空间的偏移可以访问设备的配置空间。

PCI配置空间的访问有两种方法：CPU I/O和MMIO.


CPU I/O
-------

CPU I/O即为用x86的in/out系列指令访问，方法是往0xcf8写入地址，然后通过0xcfc端口读写数据。

地址的编码为 ``0x80000000 | (b << 16) | (d << 11) | (f << 8) | offset``. 如果偏移地址不是4字节对齐，则可能需要访问 0xcfc+offset 端口。

具体见 coreboot 代码的 `src/arch/x86/include/arch/pci_io_cfg.h`.


MMIO
----

使用MMIO方式，需要先配置MMIO地址，方法是用CPU I/O方式配置PCIEXBAR. PCIEXBAR位于00:00.0配置空间的0x60地址，配置方式是往其中写入 ``CONFIG_MMCONF_BASE_ADDRESS | 4 | 1``. 此后MMIO的地址即为 ``CONFIG_MMCONF_BASE_ADDRESS``. 一般这个地址在 bootblock 阶段配置。

用MMIO访问设备配置空间的某个地址，直接读写 ``PCIEXBAR | (b << 20) | (d << 15) | (f << 12) | offset`` 即可，偏移需要根据要读的数据宽度对齐。

具体见 coreboot 代码的 `src/arch/x86/include/arch/pci_mmio_cfg.h`.
