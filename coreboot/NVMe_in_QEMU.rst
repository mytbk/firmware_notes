Emulate an NVMe device in QEMU
==============================

In QEMU source code, tests/nvme-test.c contains following lines::

  qtest_start("-drive id=drv0,if=none,file=/dev/null,format=raw "
  "-device nvme,drive=drv0,serial=foo");

That's the way to emulate an NVMe device. We need to specify a drive
with an ID, set interface **if=none**, then use **-device nvme** to
add an NVMe device to the virtual machine (use **-device help** to see
what devices can add to the QEMU VM).
