

#include "Bootloader.h"
#include "CommDevice.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define DEFAULT_XRES_GPIO   (4)                   // corresponds to GPIO B on iMX6
#define DEFAULT_DEVICE_NAME ("/dev/spidev0.2")
#define DEFAULT_SPEED       (1600000)             // 1.6MHz


const char help[] = "bootloader-tool [-r XRES-GPIO] [-d DEVICE] [-s SPEED] file.cyacd"
                    "\n\tXRES-GPIO\t- gpio in the form for export in /sys/class/gpio"
                    "\n\tDEVICE\t- the full pathname of the SPI/I2C/UART device, e.g /dev/spidev0.2"
                    "\n\tSPEED\t- speed in bps."
                    "\n";

int main(int argc, char **argv)
{
    int xres_gpio = -1;
    const char *device_name = NULL;
    int speed = -1;

    int opt;
    while(opt = getopt(argc, argv, "x:d:s:"))
    {
        switch(opt)
        {
        case 'x':
            if(xres_gpio >= 0)
            {
                fprintf(stderr, "XRES GPIO (-x) already specified. Aborting.");
                exit(EXIT_FAILURE);
            }

            xres_gpio = atoi(optarg);
            break;

        case 'd':
            if(device_name)
            {
                fprintf(stderr, "Device (-d) already specified. Aborting.");
                exit(EXIT_FAILURE);
            }

            device_name = optarg;
            break;

        case 's':
            if(speed >= 0)
            {
                fprintf(stderr, "Speed (-s) is already specified. Aborting.");
                exit(EXIT_FAILURE);
            }
            speed = atoi(optarg);
            break;

        default:
            fprintf(stderr, "Unknown option -%c. Aborting.", char(opt));
            exit(EXIT_FAILURE);
        }
    }

    if(optind >= argc)
    {
        fprintf(stderr, "Expected a .cyacd file name. Aborting.");
        exit(EXIT_FAILURE);
    }

    const char *cyacd_filename = optarg;

    if(xres_gpio < 0)
        xres_gpio = DEFAULT_XRES_GPIO;
    if(!device_name)
        device_name = DEFAULT_DEVICE_NAME;
    if(speed < 0)
        speed = DEFAULT_SPEED;

    // detecting the device type
    CommDevice *device;
    if(strstr(device_name, "/dev/spi") == device_name)
    {
        device = new SpiDevice(device, speed);
    }
    else if(strstr(device_name, "/dev/i2c") == device_name)
    {
        fprintf(stderr, "Device i2c not supported yet. Aborting.");
        exit(EXIT_FAILURE);
    }
    else if(strstr(device_name, "/dev/tty") == device_name)
    {
        fprintf(stderr, "Device tty not supported yet. Aborting.");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "Unknown device. Aborting.");
        exit(EXIT_FAILURE);
    }


    exit(EXIT_SUCCESS);
}
