/* 0x180002780 is gSmmRt, 0x180002788 is gSmmServ */

fcn_t ecio_if[] = {
	fcn_180001ae4, fcn_180001b20, fcn_180000a78, fcn_180000ac0,
	0x0000000180000b48ULL, 0x0000000180000ba0ULL, 0x0000000180000be8ULL,
   	0x0000000180000c30ULL, 0x0000000180000c68ULL, 0x0000000180000cb0ULL, 
	0x0000000180000954ULL, 0x0000000180000820ULL, 0x00000001800027a0ULL
};

entry()
{
	/*
	gSmmServ->Register();
	gSmmServ->Register();
	*/
	gSmmRt->InstallProtocolInterface(..., &guid_7310e28e, 0, &ecio_if);

	/* ... */
}

fcn_180000ac0(uint8_t *a0, uint8_t a1, uint8_t a2, a3)
{
	[rsp+0x20] = a3;
	eax = 0x1e;
	ebx = 2;
	r9d = a2;
	sil = a1;
	rdi = a0;
	if (a2 > 0x1e) {
		r9d = 0x1e;
	}
	rax = a0+0x60;
	r10d = r9b;
	byte [rsp+0x18] = r9b;
	[rax] = dl;
	r10d += ebx;

	if (r10d > ebx) {
		r8 = rsp+0x18;
		do {
			rcx = qword a0[0x60];
			r8 += 8;
			al = byte [r8];
			byte rcx[bl] = al;
			bl++;
		} while (bl < r10d);
	}
	fcn.180000798();
	r8b = bl;
	int ret = fcn.1800003ac(&a0[0x60], a1, r8b);
	fcn.180000714();
	return ret;
}

fcn.1800003ac(uint8_t *a0, uint8_t a1, uint8_t a2)
{
	a0[0] = a1;
	int ret = fcn.180000348(a0, a2);
	if (ret < 0)
		return ret;
	ret fcn.180000280(0, a0, 0x10);
}

fcn.180000798()
{
	if ([0x180002740] == 0)
		return;

	rbx = 0x84;
	edi = 4;
	do {
		r9d = eax = pci_read(0xf800, rbx); /* read LPC_GEN*_DEC */
		eax &= 0xfffc; /* decode range base address */
		r9d &= 0xfc0000; /* decode range address mask */
		if (eax == 0x920) {
			if (r9d < 0x200000) {
				r9d = 0x1c0000;
			} else {
				r9d = 0x200000;
			}
			r9d |= 0x900;
			pci_clear_set_bits(0xf800, rbx, 0xfcfffc, r9d);
		}
		rbx += 4;
		rdi--;
	} while (rdi != 0);
}

fcn.180000714()
{
	if ([0x180002740] == 0)
		return;

	rbx = 0x84;
	edi = 4;
	do {
		r9d = eax = pci_read(0xf800, rbx); /* read LPC_GEN*_DEC */
		eax &= 0xfffc; /* decode range base address */
		r9d &= 0xfc0000; /* decode range address mask */
		if (eax == 0x900) {
			if (r9d > 0x200000) {
				r9d -= 0x200000;
			} else {
				r9d = 0;
			}
			r9d |= 0x920;
			pci_clear_set_bits(0xf800, rbx, 0xfcfffc, r9d);
		}
		rbx += 4;
		rdi--;
	} while (rdi != 0);
}

pci_read(uint32_t bdf, uint32_t offset) /* 0x1800020a8, bdf is (b<<16)|(d<<11)|(f<<8) */
{
	bdf &= 0xffffff00;
	uint32_t addr = ((bdf + offset) & 0xfffffffc) | 0x80000000;
	outl(addr, 0xcf8);
	return inl(0xcfc + (offset & 3));
}

pci_clear_set_bits(uint32_t bdf, uint32_t offset, uint32_t clr_bits, uint32_t set_bits) /* 0x1800020d4 */
{
	bdf &= 0xffffff00;
	clr_bits = ~clr_bits;
	uint32_t addr = ((bdf + offset) & 0xfffffffc) | 0x80000000;
	outl(addr, 0xcf8);

	uint16_t cfc_addr = 0xcfc + (offset & 3);
	eax = inl(cfc_addr);
	eax &= clr_bits;
	eax |= set_bits;
	outl(eax, cfc_addr);
}

fcn.180000348(uint8_t * a0, uint8_t a1)
{
	int ret = 0; /* @ r8 */
	al = a1;
	rbx = a0;
	if (a1 != 0) {
		if (a1 <= 0x1e) {
			ret = fcn.1800002dc(2, a0+2, a1);
		} else {
			ret = -E...;
		}
	}
	cl = a0[0];
	outb(0, 0x910);
	outb(cl, 0x911);
	do {
		outb(0, 0x910);
		al = inb(0x911);
	} while (al != 0);
	return ret;
}

fcn.180000280(uint8_t a0, uint8_t *a1, uint8_t a2)
{
	if (a0 >= 0x20)
		return -E...;

	if (a0 + a2 > 0x20)
		return -E...;
	if (a2 == 0)
		return 0;

	do {
		r9 = 0x180002540; /* r9[i] = 0x10+i */
		outb(r9[a0], 0x910);
		a0++;
		*a1 = inb(0x911);
		a1++;
		a2--;
	} while (a2);
	return 0;
}

fcn.1800002dc(uint8_t a0, uint8_t *a1, uint8_t a2)
{
	if (a0 >= 0x20)
		return -E...;

	if (a0 + a2 > 0x20)
		return -E...;
	if (a2 == 0)
		return 0;

	do {
		r9 = 0x180002540; /* r9[i] = 0x10+i */
		outb(r9[a0], 0x910);
		outb(*a1, 0x911);
		a0++;
		a1++;
		a2--;
	} while (a2);
	return 0;
}

