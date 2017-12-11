shutdown an Intel-based machine
-------------------------------

Use the PM functionality of Intel-PCH, which is also used in ACPI/SMM:

  mov eax, 0x3c00
  mov dx, PMBASE+4
  out dx, eax

PMBASE can be read from a running machine in PCI configuration space of 00:1f.0 offset 0x40. You can also disassemble the vendor firmware. It's usually configured in bootblock or PEI phase.

Replace the instructions of vendor firmware with the instructions to shutdown the machine, then we can know if the firmware code is really run.
