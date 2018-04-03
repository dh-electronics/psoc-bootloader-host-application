#pragma once


class CommDevice
{
public:
    virtual ~CommDevice() {}

    virtual bool open()     =0;
    virtual void close()    =0;
    virtual int read()      =0;
    virtual int write()     =0;

};
