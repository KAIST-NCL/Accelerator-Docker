#ifndef _ACC_MANAGER_MANAGER
#define _ACC_MANAGER_MANAGER

#include "utils.hpp"
#include "device-detector.hpp"

#define ERR_CODE_DEVICE_LOCKED 10
#define ERR_CODE_DEVICE_NOT_FOUND 11
#define ERR_CODE_PARSE_DETECT_DEVICES_FAILED 12

class Manager{
    public:
        Manager(Context*,Container*);
        bool requestDevices();
        bool requestDevice(Device);
        bool releaseDevices();
        bool parseAndDetectDevices(list<Device>*);
    private:
        Context *ctx;
        Container* cont;
        string status_file;

        bool updateStatusFile();
};

#endif