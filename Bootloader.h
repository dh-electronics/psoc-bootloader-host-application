// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: (c) 2018-2023 DH electronics GmbH

#pragma once


#include "CommDevice.h"



#define XRES_GPIO_DEFAULT   ( 4)                  // corresponds to GPIO B
#define XRES_GPIO_INVALID   (-1)
#define XRES_GPIO_SUPRESS   (-2)                  // no gpio pin will be used
#define XRES_GPIO_LABEL     (-3)                  // reference GPIO by label

class Bootloader
{
public:
    Bootloader(CommDevice *dev, uint16_t xresPin, const char *xres_gpio_label, uint32_t expSiId, uint32_t expSiRev);
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
    const int16_t               xresPin_;               //< gpio number (sysfs)
    const char                 *xres_gpio_label_;        //< gpio label (libgpiod) (A..Z)

    static Bootloader *         self_;      // singleton
};
