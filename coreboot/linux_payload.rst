Using Linux kernel as coreboot payload
======================================

Using Linux kernel as coreboot payload is not documented in detail in
coreboot wiki. I'll do it step by step to show how to use Linux kernel
as coreboot payload, then use kexec to boot to the system on disk.

Configuring the kernel
----------------------

We need to make a small kernel so that it can fit into the flash
chip. Therefore, I use ``make allnoconfig`` to configure a minimized
kernel configuration.

To make the kernel useful, we need to enable printk.

::

  -> General setup
     -> Configure standard kernel features (expert users)
        -> [*]   Enable support for printk

We'll also need to let kernel bring up the devices that kernel can
print messages to and enable console support.

::

  -> Device Drivers
    -> Character devices
      -> [*] Enable TTY
      -> [*] TTY driver to output user messages via printk
      -> Serial drivers
        -> [*] 8250/16550 and compatible serial support
        -> [*]   Console on 8250/16550 and compatible serial port

Then we make the kernel and configure coreboot to make the generated
bzImage as payload, and set command line as
``console=ttyS0,115200``. Remember to set the correct ROM size and
CBFS size.

To test it in QEMU, run::

  qemu-system-x86_64 -m 2G -bios build/coreboot.rom -serial stdio

Then we can see the kernel boot messages.
