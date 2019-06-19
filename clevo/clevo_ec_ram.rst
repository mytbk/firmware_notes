Clevo W650SZ 的 EC RAM
==============================

W650SZ 的 EC 使用常规的 0x62/0x66 端口，ectool 可以轻松找到 EC RAM 的内容，但是看它原厂固件的 ACPI 代码，却发现一些奇怪的地方。在 EC 设备之下，有一个 OpRegion 是一个内存区域::

  OperationRegion (RAM, SystemMemory, 0xFF700100, 0x0100)

而且 EC 的一些 ACPI 操作读了这个区域里面的一些数据，例如 AC 的状态来自于这个内存区域的内容，却没有看到有其他操作写这些数据。

在疑惑中，我上网搜了关键字 SystemMemory, 0xFF700100，发现有人在 kernel 的 Bugzilla 上提过一个关于电池放电速率的一个 bug，机器是和 W650SZ 比较相似的 W740SU. 链接在 https://bugzilla.kernel.org/show_bug.cgi?id=88891

由于 RAM 区域和后面一个 EmbeddedControl 类型的区域地址能连接起来，而且 RAM 区域的大小是 256 字节，因此我认为 EC RAM 可能以 MMIO 的形式映射到了这个区域，并且固件在 e820 中保留了这个区域。于是我刷回原厂固件，读出 0xFF700100 的内容::

  #include <stdio.h>
  #include <fcntl.h>
  #include <unistd.h>
  
  int main()
  {
  	int fd = open("/dev/mem", O_RDONLY);
  	char buf[256];
  	lseek(fd, 0xFF700100, SEEK_SET);
  	read(fd, buf, 256);
  	for (size_t i = 0; i < 16; i++) {
  		for (size_t j = 0; j < 16; j++) {
  			printf("%02hhx ", buf[i*16+j]);
  		}
  		puts("");
  	}
  }

和 ``ectool -d`` 的结果比对了一下，发现这个区域的确就是 EC RAM. 刷回 coreboot 之后再读这个区域，内容就都变成了 0xff，说明固件需要配置 EC RAM 的 MMIO 映射，并且在固件中预留这个区域。

用 UEFITool 搜索 000170FF 这四个字节，发现只有三个模块有这个地址，它们是 AmiBoardInfo, EcCommDxe, EcCommSmm，以后再仔细探索这里面的内容。
