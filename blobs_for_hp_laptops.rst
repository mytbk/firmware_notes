About blobs needed by HP Elitebooks
===================================

As we can see from `the 2760p coreboot post <machines/2760p/2760p-coreboot.rst>`_,
HP Elitebook 2760p needs two blobs for EC. In the following, I'll describe
the structure of these blobs.

I'll use $s to represent the size of the ROM, which is used in
radare2.

Image structure
---------------

I analyze the blob at 0x780000 first. I have found that the structure is::

  <16-bit payload length> <16-bit checksum> <payload>

So then I try to find out what this checksum is. At last, I found that the checksum
used is `SYSV checksum <https://en.wikipedia.org/wiki/SYSV_checksum>`_.

I tried to modify some strings in the payload, then updated the
checksum. It worked!

The blob at 0x7ff700 has the same structure. However, 0x7fff00-0x7fff07 does not
belong to this blob image. So I'll find more out about this 8 bytes.

Image position
--------------

I have another HP Elitebook, 8470p, which also uses the KBC1126 EC. It
has a 16MB ROM. However, while the blob at 0xfff700 ($s-0x900) is the
same. The other blob is located at 0xf70000 ($s-0x90000) instead of
0x780000 ($s-0x80000) in 2760p.

The 8 bytes starting $s-0x100 (0x7fff00 in 2760p and 0xffff00 in
8470p) is different.

- In 2760p: ff f7 00 08 f8 00 07 ff
- In 8470p: ff f7 00 08 f7 00 08 ff

We know that the firmware hub maps the SPI NOR flash to the end of the
address space. So I guess the fifth byte means the position of one
blob.  Also we can notice, the sum of the first and second 16-byte
word is 0xffff, and so is the sum of the third and fourth 16-byte
word, so it can be used for verification. In my experiment, the 0xffff
sum seems to be a requirement.

I tried to change the four bytes at 0x7fff04 to f7 00 08 ff and move
the blob to 0x770000, and also change them to f0 00 0f ff and move the
blob to 0x700000. They both work.
