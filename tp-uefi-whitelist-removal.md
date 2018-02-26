## ThinkPad UEFI固件白名单去除
从Sandy Bridge一代开始，ThinkPad开始使用UEFI结构的固件，因此可以很容易地用UEFITool分析。以下可以用简单的方法去除ThinkPad UEFI固件的白名单。

**注意本文只是用于讲解UEFITool的使用方法。对于支持的coreboot的机型，建议使用自由的coreboot作为计算机的固件。**

### 操作方法

把原来的固件用编程器备份出来。如果是xx20这代机型的话，似乎用固件升级程序给的新版固件镜像也行。用UEFITool打开这个文件,搜索网卡的id或者“1802”这个Unicode字符串，可以定位到LenovoWmaPolicyDxe.efi这个DXE驱动。

然后在这个类型为File，名字是一个GUID的项上右键，Remove，删掉这个模块。

之后用File->Save image file保存到一个新的文件，然后刷这个新的文件就行了。

类似的可以把固件里面Computrace之类的功能也删掉，根据UEFI镜像里面文件名找相应模块就行了。

### 备注

- 此方案只对ThinkPad的UEFI固件有效，而且未做足够的测试。此外，这个方法非常暴力，如果被删的模块有一些初始化硬件的代码，删除了模块之后会导致硬件无法正常使用，这可能是部分机型进行此操作后网卡无法使用的原因。
- 我已经模块进行了一点逆向分析，但由于UEFI固件的一些特性，我还没能弄清楚里面做了什么。
- 传统修改方案及刷写方法请参考: [xx30 BIOS Whitelist Removal](https://github.com/bibanon/Coreboot-ThinkPads/wiki/xx30-BIOS-Whitelist-Removal)

已测试机型:
- X220 (写本文时所用的机型)
- W530 ([51nb](http://forum.51nb.com/forum.php?mod=viewthread&tid=1623560&extra=page%3D1%26filter%3Ddigest%26digest%3D1))
- T520 (本人测试)
- T430i ([测试失败，能启动但网卡启动失败](http://forum.51nb.com/forum.php?mod=viewthread&tid=1661941&extra=page%3D2%26filter%3Dtypeid%26typeid%3D4))
- X230 [Issue #4](https://github.com/mytbk/firmware_notes/issues/4)
- T430s ([51nb](http://forum.51nb.com/thread-1699770-1-6.html))
- E531
- L540 (修改固件后，开机时有蜂鸣声，[AR9462](https://item.taobao.com/item.htm?id=555762063297) WiFi正常使用)

### (以下是老版本的内容，大家可以不看)

我们在PE32 image section右键，选extract body把这个EFI程序提取到/tmp/wl.efi.

![UEFITool](pic/x220-bios.png)

由于UEFI的驱动是以函数的形式被调用的，驱动执行结束后以ret指令返回，使控制权回到调用者。因此我们可以直接把这个模块的入口点处指令直接修改为ret(0xc3).在UEFITool中可以看到EntryPoint的地址，直接用wxmedit等编辑器修改就行了。我用的是radare2:
```
r2 -w -c 'wa ret' /tmp/wl.efi
```

修改结束后覆盖掉原有的模块，还是在刚才的PE32 image section处，右键选replace body，选择刚才改好的EFI文件，看到UEFITool显示Replace,Rebuild等词时说明已经操作完成了。然后直接保存，UEFITool会提示另存为一个新文件。

然后刷那个新文件，白名单已经没了。

#### UEFIPatch

在知道了UEFITool中有一个UEFIPatch工具之后，发现用UEFIPatch更加方便。首先可以找出入口点开头的指令是sub rsp,0x38和call xxxx.于是把这几个字节的模式拿出来，之后让UEFIPatch找到这个模式，然后替换就行了，我们写如下patches.txt文件。(井号开头的是注释，可以去掉。)之后执行``UEFIPatch <ROMFILE>''，得到一个以.patched的新文件即为修改好的文件。


```
# patches.txt
# Patch string format
# FileGuid SectionType PatchType:FindPatternOrOffset:ReplacePattern 
# Please ensure that the latest symbol in patch string is space

# Possible section types:
#  PE32 image                    10
#  Position-independent code     11
#  TE Image                      12
#  DXE Dependency                13
#  Version information           14
#  User interface string         15
#  16-bit code                   16
#  Guided freeform               18
#  Raw data                      19
#  PEI Dependency                1B
#  SMM Dependency                1C
# Please do not try another section types, it can make the resulting image broken

# Possible patch types:
#  P - pattern-based, first parameter is a pattern to find, second - a pattern to replace
#  O - offset-based, first parameter is hexadecimal offset, second - a pattern to replace
# Patterns can have . as "any possible value" symbol

# LenovoWmaPolicyDxe.efi:
# replace the first two instructions to a ret and several nops
# do not remove the last SPACE of the next line
79E0EDD7-9D1D-4F41-AE1A-F896169E5216 10 P:4883EC..E8:C390909090 

```
