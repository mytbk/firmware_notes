## 系统信息的获取

我们知道``start_main()``要用``lib_get_sysinfo()``获取系统信息，那么我们就看看系统信息是怎样获取的。

``lib_get_sysinfo()``在``arch/x86/sysinfo.c``里面，其中有一个全局变量``struct sysinfo_t lib_sysinfo``.我们先看``include/sysinfo.h``里面是怎样定义``struct sysinfo_t``的。

```
struct sysinfo_t {
	unsigned int cpu_khz;
	struct cb_serial *serial;
	/* ... */
}

extern struct sysinfo_t lib_sysinfo;
```

也就是说，``sysinfo_t``保存的就是一些系统信息，``sysinfo.h``用一个extern类型的``lib_sysinfo``让其他文件知道有一个``lib_sysinfo``全局变量，从它里面可以得到系统信息。

我们看``lib_get_sysinfo()``,里面调用了``get_coreboot_info(&lib_sysinfo)``.按名字来看，它就是用于获取coreboot相关信息的。这个过程在``arch/x86/coreboot.c``里面，我们看一下。

```
int get_coreboot_info(struct sysinfo_t *info)
{
	int ret;

	/* Ensure the variable range MTRR index covering the ROM is set to
	 * an invalid value. */
	info->x86_rom_var_mtrr_index = -1;

	ret = cb_parse_header(phys_to_virt(0x00000000), 0x1000, info);

	if (ret)
		ret = cb_parse_header(phys_to_virt(0x000f0000), 0x1000, info);

	return ret;
}
```

我们看到了两个地址：0x00000000,0x000f0000.它们存放了什么，我们暂时还不清楚，我们先看``cb_parse_header``吧。

``cb_parse_header``在``libc/coreboot.c``里面，可以看出，它实现的功能和架构无关。

```
int cb_parse_header(void *addr, int len, struct sysinfo_t *info)
{
	struct cb_header *header;
	unsigned char *ptr = addr;
	void *forward;
	int i;

	for (i = 0; i < len; i += 16, ptr += 16) {
		header = (struct cb_header *)ptr;
		if (!strncmp((const char *)header->signature, "LBIO", 4))
			break;
	}

	/* We walked the entire space and didn't find anything. */
	if (i >= len)
		return -1;

	/* Make sure the checksums match. */
	if (ipchksum((u16 *) header, sizeof(*header)) != 0)
		return -1;

	if (!header->table_bytes)
		return 0;

	if (ipchksum((u16 *) (ptr + sizeof(*header)),
		     header->table_bytes) != header->table_checksum)
		return -1;

	info->header = header;

	/*
	 * Board straps represented by numerical values are small numbers.
	 * Preset them to an invalid value in case the firmware does not
	 * supply the info.
	 */
	info->board_id = ~0;
	info->ram_code = ~0;

	/* Now, walk the tables. */
	ptr += header->header_bytes;

	for (i = 0; i < header->table_entries; i++) {
		struct cb_record *rec = (struct cb_record *)ptr;

		/* We only care about a few tags here (maybe more later). */
		switch (rec->tag) {

		/* many cases */
			/* do something */
			/* break; */

		default:
			cb_parse_arch_specific(rec, info);
			break;
		}

		ptr += rec->size;
	}

	return 0;
}
```

首先看传入参数，它要读取指定地址一定长度的数据，把解析出来的东西放到info(就是我们的sysinfo)里面。

然后看代码，容易看出，它对起始地址开始的每16个字节做检查，看看是不是"LBIO"开头，如果是的话，就认为它是一个``struct cb_header``类型的数据，再对它做校验和检查。都通过了，就认为这个``cb_header``结构是合法的。在这个头部数据后，有一个表，头部的存放了表的长度``header->table_bytes``和校验和``header->table_checksum``,接着要对整个表做校验值检查。所有的检查通过后，就记录``info->header``.

我们先看看``struct cb_header``是什么，它在``include/coreboot_tables.h``里面，大家应该明白了，我们要读的表其实就是coreboot table.``struct cb_header``的结构很简单，存放着一个标志，之后就是头部长度和校验和，表长度和校验和，表的条目数量。

至于``cb_parse_header``里面在读表之前用``ptr += header->header_bytes;``而不是``ptr += sizeof(*header)``,我也不是很清楚。

接着就是读取表的每个条目了，同样在``include/coreboot_tables.h``里面定义了``struct cb_record``,它的结构更简单，就是tag和大小。

```
struct cb_record {
	u32 tag;
	u32 size;
};
```

事实上，``struct cb_board``只是每个表条目的公共开头，实际使用的时候都会把指针转成对应条目类型的数据结构。如``cb_memory``,``cb_mainboard``等。

```
struct cb_memory {
	u32 tag;
	u32 size;
	struct cb_memory_range map[0];
};
```

```
struct cb_mainboard {
	u32 tag;
	u32 size;
	u8 vendor_idx;
	u8 part_number_idx;
	u8 strings[0];
};
```
