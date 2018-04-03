#pragma once


#include "CommDevice.h"


class Bootloader
{
public:
    Bootloader(CommDevice *dev);

    bool load(const char *file);
};
