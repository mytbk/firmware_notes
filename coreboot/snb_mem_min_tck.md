## Sandy/Ivy memory tCK

In the following commit, maximum supported DDR frequency is read from
northbridge.

```
commit 9f3f9154c9f3dd1e3cfbd2703b681c3e9ddf4dc7
Author: Patrick Rudolph <siro@das-labor.org>
Date:   Tue Jan 26 20:02:14 2016 +0100

nb/intel/sandybridge/romstage: Read fuse bits for max MEM Clk

Instead of hardcoding the maximum supported DDR frequency to
800Mhz (DDR3-1600), read the fuse bits that encode this information.
```

And In most Sandy Bridge processors, the maximum frequency is DDR3
1333, so then minimal tCK is set to TCK\_666MHZ. And In most Ivy
Bridge, it's set to TCK\_800MHZ.

If you want to unlock the maximum frequency, you can patch as follows:

```
diff --git a/src/northbridge/intel/sandybridge/raminit.c b/src/northbridge/intel/sandybridge/raminit.c
index 042898d..3451beb 100644
--- a/src/northbridge/intel/sandybridge/raminit.c
+++ b/src/northbridge/intel/sandybridge/raminit.c
@@ -4284,6 +4284,7 @@ void init_dram_ddr3(spd_raw_data *spds, int mobile, int min_tck,
 #define HOST_BRIDGE	PCI_DEVFN(0, 0)
 #define DEFAULT_TCK	TCK_800MHZ
 
+#if 0
 static unsigned int get_mem_min_tck(void)
 {
 	u32 reg32;
@@ -4342,6 +4343,7 @@ static unsigned int get_mem_min_tck(void)
 			return TCK_400MHZ;
 	}
 }
+#endif
 
 void perform_raminit(int s3resume)
 {
@@ -4354,5 +4356,5 @@ void perform_raminit(int s3resume)
 
 	timestamp_add_now(TS_BEFORE_INITRAM);
 
-	init_dram_ddr3(spd, 1, get_mem_min_tck(), s3resume);
+	init_dram_ddr3(spd, 1, TCK_1333MHZ, s3resume);
 }
```

Well, it may cause some problems to northbridge, so use it at your own risk.
