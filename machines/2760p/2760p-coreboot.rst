HP Elitebook 2760p coreboot status
==================================

Finally, I managed to make coreboot run on my 2760p.

Some work has been done almost a year ago, but I didn't have time and
find few clues to help me do further. I had uploaded a draft that can
reserve some space in bootblock so that I can put the EC blob in
it. (`Now I've published it on
gerrit. <https://review.coreboot.org/#/c/14208/>`_)

In the past few days, I do the region test again, mainly in SEC core,
to find out about the region that is critical for the machine. What I
found is:

* 0x7ff600-0x7ff6ff: if I modify this region, the EC looks good but
  the laptop cannot boot. I think this may be a 2048-bit RSA public
  key.

* 0x7ff700-0x7fff07: if I modify this region, the EC will brick.

So what I need to reserve is the 0x7ff700-0x7fff07. So I set
*ID_SECTION_OFFSET* be 0x900, which is the distance from 0x7ff700 to
the end of memory.

Also there is a padding for EC before the volume that stores the PEI
phase at 0x780000, so I tried to set the CBFS size be the size of this volume,
0x60000. 

UPDATE(2017-02-21): You can also move the blob padding before the CBFS.
Refer to `<../blobs_for_hp_laptops.rst>`_.

EHCI is also a problem. Thank phcoder for pointing out the
*USBDEBUG_HCD_INDEX* thing in the autoport document. I've tried using
EHCI debug port in Linux, and know I should use dbgp1, whose PCI
address is 00:1d.0, so I should set *USBDEBUG_HCD_INDEX* to 1 instead
of 2.

At last I can boot this laptop with coreboot, though there are still a
lot of things to solve.

How to build it?
----------------

Here's some note about how to build coreboot for HP Elitebook 2760p.

First backup your factory firmware.

Get the code and build it. If you don't want to move the blobs,
remember to set the CBFS size to 0x60000::

 git fetch origin refs/changes/41/18241/10
 git checkout FETCH_HEAD
 make nconfig
 make

Get the EC data from the factory firmware::

 dd if=2760p-factory.rom of=ec-data.bin bs=1 skip=8386304 count=2048
 dd if=2760p-factory.rom of=ec-fw.bin bs=1 skip=7864320 count=65536

Use `hp_ec_insert <https://github.com/mytbk/firmware_notes/wiki/hp_ec_insert.c>`
to write them to build/coreboot.rom. I'll put the firmware block at 0x780000
to the beginning of the BIOS region before the 2MB CBFS size::

 ./hp_ec_insert build/coreboot.rom ec-data.bin ec-fw.bin -0x900 -0x300000

At last flash the BIOS part of build/coreboot.rom.
