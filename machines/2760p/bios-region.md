## HP Elitebook 2760p的BIOS区域测试
很不幸的是，HP Elitebook 2760p的固件非常难修改。此前我改了下DXE卷的模块，我只是把里面的PE镜像的一些指令换成同义指令，结果就无法启动了，但修改bootblock或者PEI卷的指令却没问题，这让我怀疑DXE卷被签名了。此外，我尝试刷coreboot后，插入电池之后电量指示灯不断地闪，无法给系统上电，说明很有可能EC和BIOS放在同一块芯片上。于是我尝试做个区域改写的测试，看看哪些区域是能写的，以下是部分结果。

UEFITool分析出来的几个padding区域是可以改写的，改写后可以正常启动。还有一个unknown区域，里面存放的是DMI信息，修改后正常启动，DMI信息被修改。

修改DXE卷，插入电池后正常，启动失败。

接下来是给BIOS的最高地址处填零，最后发现填了最后256B后，电池插入后指示灯不断地闪，而填了最后32B后没问题。

最后我还试了下改写ME固件，填掉了ME固件的头1M后，系统正常启动，不过也出现了半小时后自动关机的现象。

### update at 2016.04.03

后来发现UEFITool找到的那个0x108000的padding区域非常可疑，因为将前面的区域用随机值填写后，插入电池后仍然正常，但修改了此区域的部分数据后，插入电池后电量指示灯就不断地闪。于是我要对它进行分析。

我用UEFITool提取了这部分区域，然后用wxmedit看了一下，发现里面很多地方都是0xff，中间有几片非0xff的区域，其中还有一些有意义的字符串。我用strings看了一下，发现一些比较有意思的内容：

```
A01/26/15 koutfld  Copyright 2003 - 2010 by Hewlett-Packard Company.x
```

```
2DeepSleep
Exiting DeepSleep
RTC power loss=%bu
**** 8051 RESET ****
Kernel:  %02bX.%02bX, 
 Checksum=%04X
Machine: %02bX.%02bX, 
DeviceId=%02bXh
OnState()
Before OnState loop
OffState()
StandbyState()
Exited StandbyState() loop
SetupWakeup
```

```
DebugTimer=%u
```

```
[5m**Bus:%bd,Mux:%02bXh,Addr:%02bXh,AbError:%02bXh,AbClock:%02bXh
```

```
Vcc1Init
Vcc2Init
DisableVcc2Outputs
EnableVcc2Outputs
```

```
V=%u,I=%d,P=%u,PMC=%u
PMC Calibration Snapshot taken
```

```
IsAmtFlashUnlock()=%bd
```

```
PECI timeout error
Cmd FCS error: %02bXh,%02bXh
PECI: All data is zero!
Data FCS error: %02bXh,%02bXh
PECI bad completion code %02bXh
Error reading Tjmax via PECI
PECI bad temperature: MSB=%02bX, LSB=%02bXh
PL1=%bu,PL2=%bu
```

```
C:\BuildACMs_BIOSAC__Rev1.0_RC2\Projects\Snb\BIOSAC\BUILD\Release\BIOSAC_REL.pdb
```

尤其是最后一个，可以发现这段代码可能来自一个叫BIOSAC的项目，而且还说明了是SNB平台。但是在Google上也查不到相关的信息，只好先不研究了。
