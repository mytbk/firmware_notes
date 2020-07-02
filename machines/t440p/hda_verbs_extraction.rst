Extracting HDA Verbs from UEFI
=========================================

High Definition Audio [1]_ is an infrastructure to support high quality audio in a PC environment. The Intel PCH has an HD audio controller that communicates with external codecs.

The coreboot port of T440p has a problem of audio output with a headphone. To investigate in this, I try to find out the HDA verbs the factory firmware configures.

In coreboot the HDA verbs are represent as some macros. To see what these macros expand to, we can check out src/include/device/azalia_device.h::

  #define AZALIA_PIN_CFG(codec, pin, val)			\
	(((codec) << 28) | ((pin) << 20) | (0x71c << 8)	\
		| ((val) & 0xff)),			\
	(((codec) << 28) | ((pin) << 20) | (0x71d << 8)	\
		| (((val) >> 8) & 0xff)),		\
	(((codec) << 28) | ((pin) << 20) | (0x71e << 8)	\
		| (((val) >> 16) & 0xff)),		\
	(((codec) << 28) | ((pin) << 20) | (0x71f << 8)	\
		| (((val) >> 24) & 0xff))

So AZALIA_PIN_CFG is expanded to four 32-bit words. We can also see some special bits 0x71c to 0x71f. We can search these in the factory UEFI firmware.

In T440p, there is ``AZALIA_PIN_CFG(0, 0x12, 0x90a60130)``, that is 0x01271c30, 0x01271d01, 0x01271ea6, 0x01271f90. We open the firmware image with UEFITool and search hex pattern 301c2701, then find PchConfigDxe.

Then we can use radare2 or Ghidra to further analyze this UEFI module. The T440p HDA verbs is in `t440p_verbs.txt <t440p_verbs.txt>`__.
  
.. [1] https://www.intel.com/content/www/us/en/standards/high-definition-audio-specification.html
