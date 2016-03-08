## Quark BSP中GRUB legacy的编译

GRUB legacy是个很长时间都没更新的项目了，而Intel Quark BSP给GRUB
legacy打了些补丁让其支持EFI.但是用比较新的工具链构建的时候还是会出问题。

首先是GCC的问题，GCC 5开始默认使用C11，但是GRUB legacy的一些inline代码
只能用C89编译，否则连接时会出错。

然后是GNU EFI的问题，Quark BSP Build Guide里面用的是3.0u版本，我使用最
新的3.0.3版本的时候，总是出现setjmp/longjmp的连接问题，我开始以为是编
译器的问题，后来才发现GNU EFI里面有sjlj的实现，但是两个版本sjlj所在的
静态库不一样，需要修改一下efi/Makefile.am，加上@LIBEFI@.

```
grub.so: $(GRUBSO_OBJS) $(GRUBSO_LIBS) @LIBGNUEFI@ @LIBEFI@
```
