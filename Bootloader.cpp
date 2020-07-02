#include "Bootloader.h"
#include "hal/System.h"
#include "hal/GPIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>


extern "C"
{
#include "cybtldr_api.h"
#include "cybtldr_api2.h"
}


using namespace dhcom;


Bootloader *  Bootloader::self_ = NULL;
static CyBtldr_CommunicationsData commData;


static const char* getBlErrorString(int err)
{
    if(CYRET_ERR_COMM_MASK & err)
    {   // DHCOM_HAL errors
        switch(STATUS(err & ~CYRET_ERR_COMM_MASK))
        {
        case STATUS_DEVICE_DOESNT_EXIST:
            return "The requested I/O device does not exist for this target hardware";
        case STATUS_DEVICE_OPEN_FAILED:
            return "Opening I/O device failed";
        case STATUS_DEVICE_CLOSE_FAILED:
            return "Closing I/O failed";
        case STATUS_DEVICE_NOT_OPEN:
            return "device must be open to perform the requested operation";
        case STATUS_DEVICE_ALREADY_OPEN:
            return "trying to open the already open device";
        case STATUS_DEVICE_READ_FAILED:
            return "reading from the device failed";
        case STATUS_DEVICE_WRITE_FAILED:
            return "writing to the device failed";
        case STATUS_DEVICE_CONFIG_FAILED:
            return "attempt to configure the device failed (parameters invalid)";
        case STATUS_I2C_SLAVE_SELECT_FAILED:
            return "I2C Slave select failed";
        default:
            return "Communication Interface unknown error";
        }
    }


    if(CYRET_ERR_BTLDR_MASK & err)
    {   // Bootloader errors
        switch(err & ~CYRET_ERR_BTLDR_MASK)
        {
        case CYBTLDR_STAT_ERR_KEY:
            return "The provided key does not match the expected value";
        case CYBTLDR_STAT_ERR_VERIFY:
            return "The verification of flash failed";
        case CYBTLDR_STAT_ERR_LENGTH:
            return "The amount of data available is outside the expected range ";
        case CYBTLDR_STAT_ERR_DATA:
            return "The data is not of the proper form";
        case CYBTLDR_STAT_ERR_CMD:
            return "The command is not recognized";
        case CYBTLDR_STAT_ERR_DEVICE:
            return "The expected device does not match the detected device";
        case CYBTLDR_STAT_ERR_VERSION:
            return "The bootloader version detected is not supported";
        case CYBTLDR_STAT_ERR_CHECKSUM:
            return "The checksum does not match the expected value";
        case CYBTLDR_STAT_ERR_ARRAY:
            return "The flash array is not valid";
        case CYBTLDR_STAT_ERR_ROW:
            return "The flash row is not valid";
        case CYBTLDR_STAT_ERR_PROTECT:
            return "The flash row is protected and can not be programmed";
        case CYBTLDR_STAT_ERR_APP:
            return "The application is not valid and cannot be set as active";
        case CYBTLDR_STAT_ERR_ACTIVE:
            return "The application is currently marked as active";
        default:
        case CYBTLDR_STAT_ERR_UNK:
            return "An unknown error occurred";
        }
    }


    switch(err)
    {   // Bootloader host errors
    case CYRET_ERR_FILE:
        return "File is not accessible";
    case CYRET_ERR_EOF:
        return "Reached the end of the file";
    case CYRET_ERR_LENGTH:
        return "The amount of data available is outside the expected range";
    case CYRET_ERR_DATA:
        return "The data is not of the proper form";
    case CYRET_ERR_CMD:
        return "The command is not recognized";
    case CYRET_ERR_DEVICE:
        return "The expected device does not match the detected device";
    case CYRET_ERR_VERSION:
        return "The bootloader version detected is not supported";
    case CYRET_ERR_CHECKSUM:
        return "The checksum does not match the expected value";
    case CYRET_ERR_ARRAY:
        return "The flash array is not valid";
    case CYRET_ERR_ROW:
        return "The flash row is not valid";
    case CYRET_ERR_BTLDR:
        return "The bootloader is not ready to process data";
    case CYRET_ERR_ACTIVE:
        return "The application is currently marked as active";
    default:
    case CYRET_ERR_UNK:
        return "An unknown error occurred";
    case CYRET_ABORT:
        return "The operation was aborted";
    }
}


static const char * getGpioErrorString(STATUS status)
{
    switch(status)
    {
    case STATUS_DEVICE_OPEN_FAILED:		return "Opening I/O device failed";
    case STATUS_DEVICE_CLOSE_FAILED:		return "Closing I/O failed";
    case STATUS_DEVICE_NOT_OPEN:		return "Device must be open to perform the requested operation";
    case STATUS_DEVICE_ALREADY_OPEN:		return "Trying to open the already open device";
    case STATUS_DEVICE_READ_FAILED:		return "Reading from the device failed";
    case STATUS_DEVICE_WRITE_FAILED:		return "Writing to the device failed";
    case STATUS_DEVICE_CONFIG_FAILED:		return "Attempt to configure the device failed (parameters invalid)";
    default:                                return "Unknown error";
    }
}


static void progressUpdate(uint8_t arrayId, uint16_t rowNum)
{
    printf("Array: %hhd\tRow: %hd  \r", arrayId, rowNum); fflush(stdout);
}



Bootloader::Bootloader(CommDevice *dev, uint16_t xresPin, const char *xres_gpio_label, uint32_t expSiId, uint32_t expSiRev)
    : device_(dev)
    , expSiId_(expSiId)
    , expSiRev_(expSiRev)
    , blVer_(0)
    , xresPin_(xresPin)
    , xres_gpio_label_(xres_gpio_label)
{
    assert(!self_);
    commData.CloseConnection = closeConnection;
    commData.OpenConnection = openConnection;
    commData.ReadData = readData;
    commData.WriteData = writeData;
    commData.MaxTransferSize = dev->getMaxTransferSize();
    self_ = this;
}


Bootloader::~Bootloader()
{
    self_ = NULL;
}


static bool resetDevice(GPIO *xres)
{
    STATUS st = xres->set(false);    // pulling low
    if(st != STATUS_SUCCESS)
    {
        fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
        return false;
    }

    usleep(50000);  // 50 mS be in reset

    st = xres->set(true);     // pulling high
    if(st != STATUS_SUCCESS)
    {
        fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
        return false;
    }
    printf("...done\n");
    return true;
}


int Bootloader::load(const char *file)
{
    GPIO *xres = NULL;
    if(xresPin_ != XRES_GPIO_SUPRESS)
    {
        if (xres_gpio_label_ != NULL and xresPin_ == XRES_GPIO_LABEL)
        {
            System system = System();
            system.setHardware(System::detectSoC());

            std::string s(xres_gpio_label_);
            for (auto & c: s) c = toupper(c);
            int gpio_index = int(s.at(0) - 'A');

            xres = new GPIO(system, GPIO::PORT(gpio_index));
        }
        else
            xres = new GPIO(xresPin_);

        if(!xres)
        {
            fprintf(stderr, "\nCannot create XRES GPIO\n");
            return EXIT_FAILURE;
        }
    }

    int retcode = EXIT_FAILURE;
    do
    {
        if(xres)
        {
            printf("\nResetting device...\n");
            STATUS st = xres->open();
            if(st != STATUS_SUCCESS)
            {
                fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
                break;
            }

            st = xres->setDirection(GPIO::DIRECTION_OUTPUT);
            if(st != STATUS_SUCCESS)
            {
                fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
                break;
            }

            resetDevice(xres);
            usleep(100000); // wait 100mS for PSOC to boot
        }

        printf("\nProgramming/Verifying file %s...\n", file);
        int err = CyBtldr_Program(file, NULL, 0, &commData, progressUpdate);
        if(err != CYRET_SUCCESS)
        {
            fprintf(stderr, "...failed with: %s.\n", getBlErrorString(err));
            break;
        }
        printf("...done\n");
        retcode = EXIT_SUCCESS;
    }
    while(false);

    if(xres)
    {
        xres->close();
        delete xres;
    }

    return retcode;
}


int Bootloader::openConnection()
{
    assert(self_);
    return self_->device_->open();
}


int Bootloader::closeConnection()
{
    assert(self_);
    return self_->device_->close();
}


int Bootloader::readData(uint8_t *buf, int bytes)
{
    assert(self_);
    usleep(50000);  // 20 mS wait for Flash to be written
    return self_->device_->read(buf, bytes);
}


int Bootloader::writeData(uint8_t *buf, int bytes)
{
    assert(self_);
    usleep(1000);  // 20 mS wait for Flash to be written
    return self_->device_->write(buf, bytes);
}
