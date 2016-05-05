## 让OVMF自动执行模块

我们现在的任务是让UEFI固件启动时执行自己写的EFI程序。

首先，我们把要自动加载的模块的``MODULE_TYPE``设成``DXE_DRIVER``. 其实``UEFI_DRIVER``也可以，但是执行阶段不同。在``[LibraryClasses]``中，我们用``UefiDriverEntryPoint``替代``UefiApplicationEntryPoint``.

之后，我们要在OvmfPkg里面的dsc和fdf文件里面都添加这个模块，这样这个模块就能进入最终的fd文件里面了。

最后是调试，使用QEMU的``-nographic``参数就行了，加个less看输出可以防止被清屏。
