/* @ 0x10 of the protocol interface */
fcn_18000057c(uint64_t *a0, int *a1)
{
	t = (a0[0] ^ a0[1]) % 53;
	uint64_t *r12 = 0x180001180;
	rbx = r12[t + 3];
	if (rbx == 0 || fcn_180000cec(rbx + 4, a0) == 0) {
		if (rax = fcn_180000d80(0x180001348, a0) == 0) {
			return -E_...;
		}
		rbx = rax - 0x18;
		t = (a0[0] ^ a0[1]) % 53;
		r12[t + 3] = rbx;
	}

	rdi = rbx;
	do {
		(call qword [rbx+0x38])(rbx, qword [rbx + 0x40], a1);
		rbx = qword [rbx + 0x28] - 0x28;
	} while (rbx != rdi);
	return 0;
}

/* a0 has a binary search tree */
fcn_180000d80(a0, uint64_t * a1)
{
	rax = a0[0];
	rbx = rax[0];
	while (rbx) {
		rax = (call [a0+0x10])(a1, rbx);
		if (rax  > 0) {
			rbx = rbx[8];
		} else if (rax == 0) {
			return rbx;
		} else {
			rbx = rbx[0];
		}
	}
	return 0;
}

fcn_1800002b0()
{
	byte [0x180001180] = 0;
	qword [0x180001188] = qword [0x180001190] = 0;
	memset(0x180001198, 0, 8*53);
	qword [0x180001340] = 0; /* 0x180001198 + 8*53 */
	fcn_180000d48(0x180001348, 0x180001340, 0x1800002a0, 0, 0);
}

fcn_180000d48(a0, a1, a2, a3, a4)
{
	a0[8] = 0;
	if (a2 == NULL)
		a2 = 0x180000d40;
	if (a3 == NULL)
		a3 = 0x180000d44;
	[a0] = a1
	[a0+0x10] = a2;
	[a0+0x18] = a3;
	[a0+0x20] = a4;
}

fcn_1800092c()
{
	int t;
	rbx = 0x180000fc0;
	while (qword [rbx]) {
		fcn.1800007c8(qword [rbx], 0x180000674, rbx, 0, &t);
		rbx++;
	}
}

/* @ 0x00 of the protocol interface */
fcn_1800007c8(uint64_t *a0, a1, a2, a3, a4)
{
	ebx = a3;
	r12 = a2;
	r13 = a1;
	rbp = a0;
	if (byte [0x180001180] == 0)
		fcn.180000724();
	if (fcn.180000dd4(0x180001348, a0) != 0) {
		/* error handling */
	}
	interface_t iface = fcn.180000310(); /* allocate a new SMEV interface */
	fcn.180000e2c(0x180001348, iface + 0x18, iface, iface);
	r8 = (a0[0]^a0[1])%53;
	0x180001198[r8] = iface;

	/* ... */

	[iface+0x38] = a1;

	/* ... */

	/* install protocol interface */
}

