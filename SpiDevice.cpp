#include "SpiDevice.h"
#include <cybtldr_utils.h>


using namespace dhcom;


SpiDevice::SpiDevice(const char *devname, int speed)
    : spi_(devname)
    , speed_(speed)
{
}


SpiDevice::~SpiDevice()
{
    spi_.close();
}


int SpiDevice::open()
{
    STATUS st = spi_.open();
    if(STATUS_SUCCESS != st)
        return CYRET_ERR_UNK;

    st = spi_.setCommParams(SPI::MODE_0, 8, speed_);
    if(STATUS_SUCCESS != st)
        return CYRET_ERR_UNK;

    return CYRET_SUCCESS;
}


int SpiDevice::close()
{
    return spi_.close() == STATUS_SUCCESS ? CYRET_SUCCESS : CYRET_ERR_UNK;
}


int SpiDevice::read(uint8_t *buf, int length)
{
    STATUS st;
    spi_.transceive(buf, NULL, length, &st);
    return STATUS_SUCCESS == st ? CYRET_SUCCESS : CYRET_ERR_UNK;
}


int SpiDevice::write(uint8_t *buf, int length)
{
    STATUS st;
    spi_.transceive(NULL, buf, length, &st);
    return STATUS_SUCCESS == st ? CYRET_SUCCESS : CYRET_ERR_UNK;
}


int SpiDevice::getMaxTransferSize() const
{
    return MAX_TRANSFER_SIZE;
}
