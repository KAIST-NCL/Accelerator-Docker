#ifndef _ACC_MANAGER_MOUNTER
#define _ACC_MANAGER_MOUNTER

#include "common.hpp"

class Mounter{
    public:
        Mounter(Container cont);
        bool mountDevice(Device device);
    private:
        Container cont;
        bool mountDeviceFile(string dev);
        bool mountLibraryFile(LibraryNode lib);
};

#endif