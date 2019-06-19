# 一些firmware手记

### coreboot,libreboot,tools
* [USB debug gadget](coreboot/usb-debug-gadget.md)
* [libpayload分析](coreboot/libpayload/00-index.md)
* [CubieTruck SPI](coreboot/cubietruck-spi.md)
* [X200刷libreboot](coreboot/x200-libreboot.md)
* [Sandy/Ivy Bridge memory frequency](coreboot/snb_mem_min_tck.md)
* [Use NVMe in QEMU](coreboot/NVMe_in_QEMU.rst)
* [Using Linux kernel as coreboot payload](coreboot/linux_payload.rst)

### UEFI学习笔记
* [edk2的编译](uefi/building-edk2.md)
* [Quark BSP GRUB legacy的编译](uefi/quark-grub-legacy.md)
* [使用OpenSSL](uefi/using-openssl.md)
* [让UEFI自动执行一个模块](uefi/uefi-driver.md)
* [(小技巧)生成UUID](uefi/genuuid.md)
* [UEFI逆向方法分析](uefi/reverse-uefi.md)
* [A possible method to trace UEFI firmware](uefi/tracing-efi.md)
* [WIP:edk2及clover的编译]
* [WIP:Tianocore payload]

### Hardware
* [shutting down an Intel machine](shutdown.rst)
* [Intel PCH registers](pch-registers.txt)

### Intel Galileo
* [Galileo Gen2 上手](galileo-gen2.md)

### [一些机器的实验笔记](machines/README.md)

#### ThinkPad firmware
* [ThinkPad UEFI固件去白名单](thinkpad/tp-uefi-whitelist-removal.md)
* [再谈 ThinkPad 白名单](thinkpad/tp-whitelist.rst)
* [TODO:coreboot with smapi?]

#### HP firmware (主要是2760p,因为它的BIOS芯片可以直接插拔)
* [Blobs for HP laptops](blobs_for_hp_laptops.rst)

#### [Clevo（蓝天）笔记本](clevo)

### 文章收藏
* [BIOS硬盘加密的逆向研究](https://jbeekman.nl/blog/2015/03/reverse-engineering-uefi-firmware/)
* [Unlocking my Lenovo laptop](http://zmatt.net/unlocking-my-lenovo-laptop-part-1/): 最早的ThinkPad xx30 EC逆向工作
* [subfate的coreboot学习系列文章](http://blog.csdn.net/subfate/article/category/6095826)
* [UEFI编程入门](http://x86asm.net/articles/uefi-programming-first-steps/)
* [SOP16与SOP8对应图](http://www.biosrepair.com/bios/TSSOP16.htm)
* [PCI Express pinout](http://www.allpinouts.org/index.php/PCI_Express_Card_and_PCI_Express_Mini_Card)
* [DIP socket as clip](https://www.flashrom.org/File:DIP_socket_as_SOIC_clip.jpg)
* [Use FreeBSD on Hikey](https://wiki.freebsd.org/arm64/HiKey)
* [X200s libreboot](https://blissjoe.com/2015/12/libreboot-x200s/)
