

#include "Bootloader.h"
#include "SpiDevice.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define DEFAULT_XRES_GPIO   (4)                   // corresponds to GPIO B on iMX6
#define DEFAULT_DEVICE_NAME ("/dev/spidev0.2")
#define DEFAULT_SPEED       (1600000)             // 1.6MHz
#define DEFAULT_SILICON_ID  (0x190011a9)
#define DEFAULT_SILICON_REV (0)


static void showHelp()
{
    printf( "bootloader-tool [-r XRES-GPIO] [-d DEVICE] [-s SPEED] [-i ID] [-r REV] file.cyacd"
           "\n\tXRES-GPIO\t- gpio in the form for export in /sys/class/gpio"
           "\n\tDEVICE\t- the full pathname of the SPI/I2C/UART device, e.g /dev/spidev0.2"
           "\n\tSPEED\t- speed in bps."
           "\n\tID\t- silicon id, 32-bit HEX."
           "\n\tREV\t- silicon revision, 32-bit HEX."
           "\n");
}


int main(int argc, char **argv)
{
    if(argc <= 1)
    {
        showHelp();
        exit(EXIT_SUCCESS);
    }


    int xres_gpio = -1;
    const char *device_name = NULL;
    int speed = -1;
    uint32_t silicon_id = 0xFFFFFFFF;
    uint32_t silicon_rev = 0xFFFFFFFF;

    int opt;
    while(opt = getopt(argc, argv, "hx:d:s:i:r:"))
    {
        const char *err_string = NULL;
        switch(opt)
        {
        case 'h':
            showHelp();
            exit(EXIT_SUCCESS);

        case 'x':
            if(xres_gpio >= 0)
            {
                err_string = "/nXRES GPIO (-x) already specified. Aborting./n";
                break;
            }

            if(1 != sscanf(optarg, "%d", &xres_gpio))
                err_string = "/nArgument conversion error for option -x. Aborting./n";
            break;

        case 'd':
            if(!device_name)
                device_name = optarg;
            else
                err_string = "/nDevice (-d) already specified. Aborting./n";
            break;

        case 's':
            if(speed >= 0)
            {
                err_string = "/nSpeed (-s) is already specified. Aborting./n";
                break;
            }

            if(1 != sscanf(optarg, "%d", &speed))
                err_string = "/nArgument conversion error for option -s. Aborting./n";
            break;

        case 'i':
            if(silicon_id != 0xFFFFFFFF)
            {
                err_string = "/nSilicon ID (-i) is already specified. Aborting./n";
                break;
            }

            if(1 != sscanf(optarg, "%ux", &silicon_id))
                err_string = "/nArgument conversion error for option -i. Aborting./n";
            break;

        case 'r':
            if(silicon_rev != 0xFFFFFFFF)
            {
                err_string = "/nSilicon revision (-r) is already specified. Aborting./n";
                break;
            }

            if(1 != sscanf(optarg, "%ux", &silicon_rev))
                err_string = "/nArgument conversion error for option -r. Aborting./n";
            break;

        default:
            fprintf(stderr, "/nUnknown option -%c. Aborting./n", char(opt));
            exit(EXIT_FAILURE);
        }

        if(err_string)
        {
            fprintf(stderr, err_string);
            exit(EXIT_FAILURE);
        }
    }

    if(optind >= argc)
    {
        fprintf(stderr, "/nExpected a .cyacd file name as the last arg. Aborting./n");
        exit(EXIT_FAILURE);
    }

    const char *cyacd_filename = optarg;

    // setting the default values where needed
    if(xres_gpio < 0)
        xres_gpio = DEFAULT_XRES_GPIO;
    if(!device_name)
        device_name = DEFAULT_DEVICE_NAME;
    if(speed < 0)
        speed = DEFAULT_SPEED;
    if(silicon_id == 0xFFFFFFFF)
        silicon_id = DEFAULT_SILICON_ID;
    if(silicon_rev == 0xFFFFFFFF)
        silicon_rev = DEFAULT_SILICON_REV;

    // detecting the comm device type
    CommDevice *device;
    if(strstr(device_name, "/dev/spi") == device_name)
    {
        device = new SpiDevice(device_name, speed);
    }
    else if(strstr(device_name, "/dev/i2c") == device_name)
    {
        fprintf(stderr, "/nDevice i2c not supported yet. Aborting./n");
        exit(EXIT_FAILURE);
    }
    else if(strstr(device_name, "/dev/tty") == device_name)
    {
        fprintf(stderr, "/nDevice tty not supported yet. Aborting./n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "/nUnknown device. Aborting./n");
        exit(EXIT_FAILURE);
    }

    Bootloader bl(device, xres_gpio, silicon_id, silicon_rev);
    const int code = bl.load(cyacd_filename);
    delete device;
    return code;
}
