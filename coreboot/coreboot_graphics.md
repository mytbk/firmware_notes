## coreboot图形显示支持总结

coreboot需要对显示适配器进行初始化后才能显示内容在屏幕上。很多私有BIOS
都有VGA BIOS用于做初始化，现代UEFI固件有一个专门的EFI驱动完成这个工作，
为了支持CSM，也会带VGA BIOS. coreboot也有图形初始化的方法。

### native graphics initialization

### coreboot VGA option ROM loading

### SeaBIOS payload VGA option ROM loading

### SeaBIOS payload with SeaVGABIOS
