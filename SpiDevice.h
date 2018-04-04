#pragma once


#include "CommDevice.h"
#include "hal/SPI.h"


class SpiDevice : public CommDevice
{
public:
    SpiDevice(const char *devname, int speed);
    virtual ~SpiDevice();

    virtual int open();
    virtual int close();
    virtual int read(uint8_t *buf, int bytes);
    virtual int write(uint8_t *buf, int bytes);
    virtual int getMaxTransferSize() const;

private:
    dhcom::SPI  spi_;
};
