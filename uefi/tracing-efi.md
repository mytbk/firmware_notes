A possible method to trace proprietary UEFI firmware
====================================================

Add a serial or EHCI debug driver. Then modify some basic drivers
which is already in edk2 source tree (e.g. CPU I/O) to let them print
out the debug information.
