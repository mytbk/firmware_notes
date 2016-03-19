## libpayload的入口

我们从libpayload的入口开始分析吧。

libpayload的代码在coreboot源码树的``payloads/libpayload``目录下。coreboot支持多种架构，因此libpayload也有架构相关代码，我们主要看x86的代码，它在``arch/x86``下。

在这个目录下有一个``libpayload.ldscript``文件，用于指导连接器连接目标文件生成使用libpayload的可执行ELF程序。我们看这个文件，可以看到入口点的符号为``_entry``.

```
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

ENTRY(_entry)
```

我们很容易地找到入口文件是``head.S``,然后可以看里面的代码。

```
_entry:
	jmp _init

/* some code */

_init:
	/* No interrupts, please. */
	cli

	/* There is a bunch of stuff missing here to take arguments on the stack
	 * See http://www.coreboot.org/Payload_API and exec.S.
	 */
	/* Store current stack pointer. */
	movl %esp, %esi

	/* Store EAX and EBX */
	movl %eax,loader_eax
	movl %ebx,loader_ebx

	/* Clear the bss */
	cld
	movl $.bss, %edi
	movl $_end, %ecx
	subl %edi, %ecx
	xor %ax, %ax
	rep stosb

	/* Setup new stack. */
	movl $_stack, %ebx

	movl %ebx, %esp

	/* Save old stack pointer. */
	pushl %esi

	/* Let's rock. */
	call start_main

	/* %eax has the return value - pass it on unmolested */
_leave:
	/* Get old stack pointer. */
	popl %ebx

	/* Restore old stack. */
	movl %ebx, %esp

	/* Return to the original context. */
	ret
```

可以看出，libpayload先进行栈的初始化，然后调用``start_main``函数，最后恢复原有的栈，用ret指令返回。我们继续看``start_main``.它在``main.c``里面，功能很简单，就是调用我们写C程序常用的main函数。

```
void start_main(void)
{
	extern int main(int argc, char **argv);

	/* Gather system information. */
	lib_get_sysinfo();

	/* Optionally set up the consoles. */
#if !IS_ENABLED(CONFIG_LP_SKIP_CONSOLE_INIT)
	console_init();
#endif

	exception_init();

	/*
	 * Any other system init that has to happen before the
	 * user gets control goes here.
	 */

	/*
	 * Go to the entry point.
	 * In the future we may care about the return value.
	 */

	(void) main(main_argc, (main_argc != 0) ? main_argv : NULL);

	/*
	 * Returning here will go to the _leave function to return
	 * us to the original context.
	 */
}
```

此外，我们注意到它用``lib_get_sysinfo()``收集系统信息，这个功能在libpayload中会很有用，之后我们会对它进行分析。
