PSoC Bootloader Host Application
================================

Usage of bootloader-tool
------------------------

**Print Help**

root@dhsom:~# psoc-bootloader-tool
```bash
psoc-bootloader-tool [-x XRES-GPIO | -n | -l LABEL-XRES-GPIO] [-d DEVICE] [-s SPEED] [-i ID] [-r REV] file.cyacd
        -n      	     - do not use XRES-GPIO
        XRES-GPIO        - gpio in the form for export in /sys/class/gpio
        LABEL-XRES-GPIO  - gpio in the form label for libgpiod"
        DEVICE  	     - the full pathname of the SPI/I2C/UART device, e.g /dev/spidev0.2
        SPEED   	     - speed in bps.
        ID      	     - silicon id, 32-bit HEX.
        REV     	     - silicon revision, 32-bit HEX.
```

**Example i.mx6qdls:** update PSoC 4 from DHCOM i.mx6qdls through SPI1 in baseboard e.g. DRC02

```bash
root@dhsom:~# psoc-bootloader-tool -x 4 -d /dev/spidev0.2 Design-570-100.cyacd
```

**Example i.mx6ULL:** update PSoC 4 from DHCOM i.mx6ULL through SPI1 in baseboard e.g. DRC02, InfoWin+NG, ...

```bash
root@dhsom:~# psoc-bootloader-tool -x 129 -d /dev/spidev0.0 Design-598-100.cyacd
```

**Example STM32MP1:** update PSoC 4 from DHCOM STM32MP1 through SPI1 in baseboard e.g. DRC02

```bash
root@dhsom:~# psoc-bootloader-tool -l B -d /dev/spidev0.0 Design-570-100.cyacd
```
_Check Device-Tree GPIO Labels if that fails._
