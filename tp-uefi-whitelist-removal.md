## ThinkPad UEFI固件白名单去除
从Sandy Bridge一代开始，ThinkPad开始使用UEFI结构的固件，因此可以很容易地用UEFITool分析。以下可以用简单的方法去除ThinkPad UEFI固件的白名单。

首先还是打开UEFITool,搜索网卡的id,可以定位到LenovoWmaPolicyDxe.efi这个DXE驱动。我们在PE32 image section右键，选extract body把这个EFI程序提取到/tmp/wl.efi.

![UEFITool](pic/x220-bios.png)

由于UEFI的驱动是以函数的形式被调用的，驱动执行结束后以ret指令返回，使控制权回到调用者。因此我们可以直接把这个模块的入口点处指令直接修改为ret(0xc3).在UEFITool中可以看到EntryPoint的地址，直接用wxmedit等编辑器修改就行了。我用的是radare2:
```
r2 -w -c 'wa ret' /tmp/wl.efi
```

修改结束后覆盖掉原有的模块，还是在刚才的PE32 image section处，右键选replace body，选择刚才改好的EFI文件，看到UEFITool显示Replace,Rebuild等词时说明已经操作完成了。然后直接保存，UEFITool会提示另存为一个新文件。

然后刷那个新文件，白名单已经没了。

注:
- 此方案只对ThinkPad的UEFI固件有效，而且未做足够的测试，如果有问题请给issue说明。
- 在学习UEFI之后，我准备再次对这个模块进行逆向分析。
- 传统修改方案及刷写方法请参考: [xx30 BIOS Whitelist Removal](https://github.com/bibanon/Coreboot-ThinkPads/wiki/xx30-BIOS-Whitelist-Removal)
