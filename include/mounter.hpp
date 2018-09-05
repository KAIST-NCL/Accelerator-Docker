#ifndef _ACC_MANAGER_MOUNTER
#define _ACC_MANAGER_MOUNTER

#include "utils.hpp"

#include <sys/mount.h>
#include <sys/types.h>

#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <fstream>

#define ERR_CODE_ONLY_ABS_PATH_ALLOWED 31

class Mounter{
    public:
        Mounter(Container cont);
        bool mountDevices(list<Device> devices);
        bool mountDevice(Device device);
    private:
        Container cont;
        bool mountDeviceFiles(list<string> devs);
        bool mountDeviceFile(string dev, char* cg_path);
        bool mountLibraries(list<LibraryNode> libs);
        bool mountLibrary(LibraryNode lib);

        bool createDev(char* src, char* dst,struct stat s);
};

#endif