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
phase, so I tried to set the CBFS size be the size of this volume,
0x60000. Fortunately, it's enough to contain a CBFS with SeaBIOS
payload.

UPDATE(2017-01-28): it seems that CBFS size can be enlarge to 0x66000,
but I'll try it with factory firmware.

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

First backup your factory firmware and write it to another flash chip.

Get the code and build it, remember to set the CBFS size to 0x60000::

 git fetch origin refs/changes/09/14209/5
 git checkout FETCH_HEAD
 make nconfig
 make

Get the EC data from the factory firmware::

 dd if=2760p-factory.rom of=ec-data.bin bs=1 skip=8386304 count=2056

Write it to build/coreboot.rom::

 dd if=ec-data.bin of=build/coreboot.rom bs=1 seek=8386304 conv=notrunc

At last flash the CBFS to the target flash chip, in my example I use a
CH341A programmer::

 flashrom -p ch341a_spi -c MX25L6436E/MX25L6445E/MX25L6465E/MX25L6473E \
   --layout cbfs-layout.txt --image cbfs -w build/coreboot.rom

where cbfs-layout.txt is::

 007a0000:007fffff cbfs

