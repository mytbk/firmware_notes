A possible method to trace proprietary UEFI firmware
====================================================

Add a serial or EHCI debug driver. Then modify some basic drivers
which is already in edk2 source tree (e.g. CPU I/O) to let them print
out the debug information.

Update: a better way would be logging in a buffer, then writing the log
to ESP.

Replace CpuIoDxe.efi
--------------------

My first attempt is to replace CpuIoDxe.efi.

I built CpuIoDxe.efi in edk2, and used it to replace the one in an OEM firmware,
and the machine boots. Then I wrote a patch to write a "hello" to log.txt in a
FAT32 partition (see [trace-cpuio.patch](trace-cpuio.patch). The machine boots and
write the log.txt as expected.
