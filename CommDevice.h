// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: (c) 2018-2023 DH electronics GmbH

#pragma once


#include <stdint.h>


class CommDevice
{
public:
    virtual ~CommDevice() {}

    virtual int open()                              =0; //< returns CYRET_SUCCESS (0) on success
    virtual int close()                             =0; //< returns CYRET_SUCCESS (0) on success
    virtual int read(uint8_t *buf, int bytes)       =0; //< returns CYRET_SUCCESS (0) on success
    virtual int write(uint8_t *buf, int bytes)      =0; //< returns CYRET_SUCCESS (0) on success
    virtual int getMaxTransferSize() const =0;
};
