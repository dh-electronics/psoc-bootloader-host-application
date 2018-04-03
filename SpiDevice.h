#pragma once


#include "CommDevice.h"
#include "hal/SPI.h"


class SpiDevice : public CommDevice
{
public:
    SpiDevice(const char *devname, int speed);
    virtual ~SpiDevice();

    virtual bool open();
    virtual void close();
    virtual int read();
    virtual int write();

private:
    dhcom::S
};
