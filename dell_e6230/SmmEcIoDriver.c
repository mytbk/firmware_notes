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
		r9d = eax = fcn.1800020a8(0xf800, rbx);
		eax &= 0xfffc;
		r9d &= 0xfc0000;
		if (eax == 0x920) {
			if (r9d < 0x200000) {
				r9d = 0x1c0000;
			} else {
				r9d = 0x200000;
			}
			r9d |= 0x900;
			fcn.1800020d4(0xf800, rbx, 0xfcfffc, r9d);
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
		r9d = eax = fcn.1800020a8(0xf800, rbx);
		eax &= 0xfffc;
		r9d &= 0xfc0000;
		if (eax == 0x900) {
			if (r9d > 0x200000) {
				r9d += 0xffe00000;
			} else {
				r9d = 0;
			}
			r9d |= 0x920;
			fcn.1800020d4(0xf800, rbx, 0xfcfffc, r9d);
		}
		rbx += 4;
		rdi--;
	} while (rdi != 0);
}

fcn.1800020a8(a0, a1)
{
	r8 = a1;
	a0 &= 0xffffff00;
	eax = a0 + a1;
	edx = 0xcf8;
	r8w &= 3;
	eax &= 0xfffffffc;
	eax |= 0x80000000;
	outl(eax, dx);
	edx = 0xcfc + r8w;
	return inl(dx);
}

fcn.1800020d4(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	a0 &= 0xffffff00;
	a2 = ~a2;
	eax = a0 + a1;
	eax &= 0xfffffffc;
	eax |= 0x80000000;
	outl(eax, 0xcf8);

	edx = 0xcfc + (a1 & 3);
	eax = inl(dx);
	eax &= a2;
	eax |= a3;
	outl(eax, dx);
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

