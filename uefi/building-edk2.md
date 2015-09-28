## edk2的编译
在Linux编译edk2是很容易的，只需要系统中的GCC工具链和ACPI编译工具iasl就行了，不过开始的时候遇到了一些麻烦，后来发现是因为以前折腾一些东西的时候把`Conf/`下的配置文件搞乱了，只要删掉`Conf/{target.txt,tools_def.txt,build_rule.txt}`，之后重新`source edksetup.sh`就行了。

编译OVMF很简单：
```
build -t GCC49 -a X64 -p OvmfPkg/OvmfPkgX64.dsc
```

还有coreboot payload:
```
build -t GCC49 -a IA32 -p CorebootPayloadPkg/CorebootPayloadPkgIa32.dsc
```
