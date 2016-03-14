## coreboot图形显示支持总结

coreboot需要对显示适配器进行初始化后才能显示内容在屏幕上。很多私有BIOS都有VGA BIOS用于做初始化，现代UEFI固件有一个专门的EFI驱动完成这个工作，为了支持CSM，也会带VGA BIOS. coreboot也有图形初始化的方法。

### native graphics initialization

对于部分硬件，coreboot有图形初始化代码。coreboot配置中有一项叫`Keep VESA framebuffer',它表示在进行图形初始化后是保持framebuffer模式还是进入text mode. 以我的理解，text mode和VGA text mode类似，只能支持ASCII字符的显示，而framebuffer则可以让屏幕的每个像素点显示任意的内容。

### coreboot VGA option ROM loading

### SeaBIOS payload VGA option ROM loading

SeaBIOS可以自动从CBFS中找到VGA BIOS并加载。一般认为SeaBIOS运行VGA BIOS的效果比coreboot要好，所以用SeaBIOS payload都不需要选择coreboot中运行VGA Option ROM的选项。不过GRUB加载SeaBIOS的效果我还没仔细研究过。

VGA BIOS加载后会进入text mode.

### SeaBIOS payload with SeaVGABIOS

SeaVGABIOS是SeaBIOS的SGA(serial graphics adapter) BIOS实现。SGA BIOS的本意是将图形显示请求(int 0x10)输出至串口，而SeaVGABIOS的功能更强大，它可以将int 0x10请求模拟为coreboot framebuffer的显示。从这里可以看出，使用SeaVGABIOS后，会保持framebuffer模式。但我没试过text mode的GRUB加载SeaBIOS的效果。
