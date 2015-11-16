## X200刷libreboot

今天终于成功地把libreboot刷入了X200,做一点总结。

首先是供电问题，之前试了ch341a编程器，树莓派，BeagleBone.它们都可以刷写不在板上的芯片，但都刷不了X200.后来到flashrom的IRC上问了一下，得知X200的BIOS芯片需要外接供电才能刷写。于是我试了好几种供电设备，用了实验室的可调电源，12V转3.3V的模块，都不能刷X200，最后用了ATX才成功。

关于ATX的说明，libreboot官方网站上已经讲述了[配置方法](http://libreboot.org/docs/install/bbb_setup.html)了。我用的是24针的ATX.首先看[Wikipedia](https://en.wikipedia.org/wiki/Power_supply_unit_%28computer%29#Wiring_diagrams)上的ATX引脚，把ATX接上电之后，要把16针(Power on)和一根地线的针脚短接(我用的是17针)，这样就能启动ATX电源。然后我用万用表确定了要用的3.3V针脚。连接时用ATX的3.3V接到测试夹的VCC上(SOIC-16的2号针)，然后把ATX的地线接到BeagleBone的一个接地针上。于是完成ATX的配置。

然后是BeagleBone的配置，看开发板的文档就行了。BeagleBone打开SPI的方法和BeagleBone Black一样，libreboot官网给的那个[参考文档](http://elinux.org/BeagleBone_Black_Enable_SPIDEV#SPI0)有说明。我用的是BeagleBone Rev A6A，看它的电路图就明白上面各个GPIO的定义。具体在[elinux网站的BeagleBone资料](http://elinux.org/Beagleboard:BeagleBone#Rev_A6A)的Schematic中。

最后是flashrom的用法。对于板外的芯片，用默认参数刷就行了，速度很快。但是对X200上的芯片，用默认的速度会读取出错，具体表现是连续两次读出的数据不一样。经过几次尝试，设置spispeed=2048是个比较好的方法，读取X2000上的8 SOIC-16芯片要一分多钟，这个速度还是不错的。libreboot给的flashrom对类似的flash芯片做了合并处理，一般不需要指定芯片型号，具体的刷写命令行是:
```
./flashrom -p linux_spi:dev=/dev/spidev1.0,spispeed=2048 -w libreboot.rom
```

最后总结一下，完整的操作流程是配置好ATX，把测试夹和ATX的那些线接好，启动BeagleBone并打开SPI，夹上夹子，用flashrom备份原来的BIOS(备份至少两次，并用file,hexdump检查一些读出的数据，确认读取正确)，最后用flashrom刷写。最后组装好机器，启动，看到标题为FREE AS IN FREEDOM的GRUB界面并能进入系统就成功了。
