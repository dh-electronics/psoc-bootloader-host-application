#pragma once


#include "CommDevice.h"


class Bootloader
{
public:
    Bootloader(CommDevice *dev, uint16_t xresPin, uint32_t expSiId, uint32_t expSiRev);
    ~Bootloader();

    int load(const char *file); //< returns EXIT_SUCCESS or EXIT_FAILURE

private:
    static int openConnection();                        //< returns CYRET_SUCCESS (0) on success
    static int closeConnection();                       //< returns CYRET_SUCCESS (0) on success
    static int readData(uint8_t *buf, int bytes);       //< returns CYRET_SUCCESS (0) on success
    static int writeData(uint8_t *buf, int bytes);      //< returns CYRET_SUCCESS (0) on success

    CommDevice  *               device_;
    const uint32_t              expSiId_;               //< expected silicon id
    const uint32_t              expSiRev_;              //< expected silicon revision
    uint32_t                    blVer_;                 //< bootloader version (read out)
    const uint16_t              xresPin_;

    static Bootloader *         self_;      // singleton
};
