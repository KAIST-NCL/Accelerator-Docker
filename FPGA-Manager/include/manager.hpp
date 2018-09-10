#ifndef _ACC_MANAGER_MANAGER
#define _ACC_MANAGER_MANAGER

#include "utils.hpp"
#include "mounter.hpp"
#include "device-detector.hpp"

#define ERR_CODE_DEVICE_LOCKED 10
#define ERR_CODE_DEVICE_NOT_FOUND 11
#define ERR_CODE_PARSE_DETECT_DEVICES_FAILED 12
#define ERR_CODE_RELEASE_DEVICE_FAILED 13

class Manager{
    public:
        Manager(Context*);
        bool requestDevices();
        bool requestDevice(list<Device>::iterator);
        bool releaseDevices();

        list<Device> getDeviceList();
    private:
        Context *ctx;
        string status_file;
        list<Device> dev_list;

        bool updateStatusFile();
        bool parseAndDetectDevices();
};

#endif