PSoC Bootloader Host Application
================================

building the bootloader-tool
----------------------------

This is a subproject of the _PSoC Host Application_ project tree. At least the
shared library libDHCOM_HAL.so is required

**Project structure**
```bash
Root of the _PSoC Host Application_ project
├──"Bootloader-Tool"
│   ├───include
│   ├───src
|   └───"CMakeLists.txt"
├───DHCOM_HAL
│   ├───include
│   ├───src
|   └───CMakeLists.txt
├───C-API
│   ├───include
│   ├───src
|   └───CMakeLists.txt
├───Tools
│   ├───include
│   ├───src
|   └───CMakeLists.txt
├───Tests
│   ├───include
│   ├───src
|   └───CMakeLists.txt
├───Examples
│   ├───include
│   ├───src
|   └───CMakeLists.txt
├─README.md
└─CMakeLists.txt
```

See README.md in _PSoC Host Application_ for build steps.

Usage of bootloader-tool
------------------------

**Help information**

root@dhsom:~# ./bootloader-tool
```bash
bootloader-tool [-x XRES-GPIO | -n] [-d DEVICE] [-s SPEED] [-i ID] [-r REV] file.cyacd
        -n      	- do not use XRES-GPIO
        XRES-GPIO   - gpio in the form for export in /sys/class/gpio
        DEVICE  	- the full pathname of the SPI/I2C/UART device, e.g /dev/spidev0.2
        SPEED   	- speed in bps.
        ID      	- silicon id, 32-bit HEX.
        REV     	- silicon revision, 32-bit HEX.
```

**Example:** update PSoC 4 from DHCOM i.mx6qdls through SPI1 in InfoWin+NG baseboard

```bash
root@dhsom:~# ./bootloader-tool -x 4 -d /dev/spidev0.2 Design-598-100.cyacd
```