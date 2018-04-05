#include "Bootloader.h"
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
    switch(err)
    {
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
    case STATUS_DEVICE_OPEN_FAILED:			return "Opening I/O device failed";
    case STATUS_DEVICE_CLOSE_FAILED:		return "Closing I/O failed";
    case STATUS_DEVICE_NOT_OPEN:			return "Device must be open to perform the requested operation";
    case STATUS_DEVICE_ALREADY_OPEN:		return "Trying to open the already open device";
    case STATUS_DEVICE_READ_FAILED:			return "Reading from the device failed";
    case STATUS_DEVICE_WRITE_FAILED:		return "Writing to the device failed";
    case STATUS_DEVICE_CONFIG_FAILED:		return "Attempt to configure the device failed (parameters invalid)";
    default:                                return "Unknown error";
    }
}


static void progressUpdate(uint8_t arrayId, uint16_t rowNum)
{
    printf("Array: %hhd\tRow: %hd  \r", arrayId, rowNum);
}



Bootloader::Bootloader(CommDevice *dev, uint16_t xresPin, uint32_t expSiId, uint32_t expSiRev)
    : device_(dev)
    , expSiId_(expSiId)
    , expSiRev_(expSiRev)
    , blVer_(0)
    , xresPin_(xresPin)
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


static bool resetDevice(GPIO &xres)
{
    printf("\nResetting device...\n");
    STATUS st = xres.open();
    if(st != STATUS_SUCCESS)
    {
        fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
        return false;
    }
    st = xres.setDirection(GPIO::DIRECTION_OUTPUT);
    if(st != STATUS_SUCCESS)
    {
        fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
        return false;
    }

    st = xres.set(false);    // pulling low
    if(st != STATUS_SUCCESS)
    {
        fprintf(stderr, "\n...failed with: %s.\n", getGpioErrorString(st));
        return false;
    }

    usleep(100);

    st = xres.set(true);     // pulling high
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
    GPIO xres(xresPin_);

    int retcode = EXIT_FAILURE;
    do
    {
        resetDevice(xres);
        usleep(100);
/*
        printf("\nErasing for file %s...\n", file);
        int err = CyBtldr_Erase(file, NULL, &commData, progressUpdate);
        if(err != CYRET_SUCCESS)
        {
            fprintf(stderr, "\n...failed with: %s.\n", getBlErrorString(err));
            break;
        }
        */
        printf("\nProgramming file %s...\n", file);
        int err = CyBtldr_Program(file, NULL, 1, &commData, progressUpdate);    // not starting the app
        if(err != CYRET_SUCCESS)
        {
            fprintf(stderr, "...failed with: %s.\n", getBlErrorString(err));
            break;
        }
        printf("...done\n\nVerifying...\n");
        err = CyBtldr_Verify(file, NULL, &commData, progressUpdate);
        if(err != CYRET_SUCCESS)
        {
            fprintf(stderr, "...failed with: %s.\n", getBlErrorString(err));
            break;
        }
        printf("...done\n");

        resetDevice(xres);
        retcode = EXIT_SUCCESS;
    }
    while(false);

    xres.close();

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
    usleep(20);
    return self_->device_->read(buf, bytes);
}


int Bootloader::writeData(uint8_t *buf, int bytes)
{
    assert(self_);
    return self_->device_->write(buf, bytes);
}
