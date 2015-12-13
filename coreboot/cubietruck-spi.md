## CubieTruck上开启SPI

我用的是主线内核，首先要在dts文件里面添加SPI的相关项。

```
&spi2 {
      pinctrl-names = "default";
      pinctrl-0 = <&spi2_pins_a>, <&spi2_cs0_pins_a>;
      status = "okay";
      spi2_0 {
             #address-cells = <1>;
             #size-cells = <0>;
             compatible = "spidev";
             reg = <0>;
             spi-max-frequency = <50000000>;
      };
};
```

注意pinctrl-0中要同时写上针脚和片选的针脚，之前少写了cs0，结果夹子夹上一块芯片后芯片都发热了。

此外为了避免spidev的一个warning，最好去掉spidev.c的几行代码。

```
diff --git a/drivers/spi/spidev.c b/drivers/spi/spidev.c
index 91a0fcd..eb5f828 100644
--- a/drivers/spi/spidev.c
+++ b/drivers/spi/spidev.c
@@ -713,11 +713,11 @@ static int spidev_probe(struct spi_device *spi)
* compatible string, it is a Linux implementation thing
* rather than a description of the hardware.
*/
-       if (spi->dev.of_node && !of_match_device(spidev_dt_ids, &spi->dev)) {
-               dev_err(&spi->dev, "buggy DT: spidev listed directly in DT\n");
-               WARN_ON(spi->dev.of_node &&
-                       !of_match_device(spidev_dt_ids, &spi->dev));
-       
}
+       /* if (spi->dev.of_node && !of_match_device(spidev_dt_ids, &spi->dev)) { */
+       /*      dev_err(&spi->dev, "buggy DT: spidev listed directly in DT\n"); */
+       /*      WARN_ON(spi->dev.of_node && */
+       /*              !of_match_device(spidev_dt_ids, &spi->dev)); */
+       /* } */

/* Allocate driver data */
spidev = kzalloc(sizeof(*spidev), GFP_KERNEL);
```

可惜的是，实际操作的时候，还是出现了SPI事务失败的信息，还需要进一步的调试。
