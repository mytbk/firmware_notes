对 mrc.asm 进行函数替换
============================

此前用 r2dumpbin 将 mrc.bin 转为 mrc.asm，下一步就是测试它能否使用。幸运的是，转出来的汇编程序只需做少量修改（去掉 NASM 的 ORG 命令，将入口点定义为全局符号），就可以汇编为目标文件，链接至 romstage，并成功运行。

接下来尝试进行函数替换，首先可以发现里面有个很明显的 printf 函数，把它相关的汇编代码去掉，然后替换为 coreboot 的 printk，测试发现串口仍然能看到 mrc.bin 的输出。

之后我开始找一些常见的库函数，首先用以下命令对 mrc.asm 出现的函数的次数进行排序::

  grep -o 'fcn_[0-9a-f]*' mrc.asm | sort | uniq -c | sort -n

然后从出现次数较多的函数开始找，可以找到 memcpy, memset 等函数，可以将其替换为 coreboot 已有的库函数。

最后重新构建 coreboot 并上机测试，没有发现异常。
