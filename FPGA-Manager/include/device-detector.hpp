#ifndef _ACC_MANAGER_DEVICE_DETECTOR
#define _ACC_MANAGER_DEVICE_DETECTOR

#include "common.hpp"

class DeviceDetector{
    public:
        DeviceDetector();
        list<Device> detect();
        list<Device> detect(list<Device> _dev);
    private:
};

class DeviceParser{
    public:
        DeviceParser(char* _configFilePath);
        list<Device> parse();
        list<Device> parse(const char* _configFilePath);
    private:
        char* configFilePath;
        bool isListValid(list<Device> deviceList);
        bool isDeviceValid(Device device);
};
#endif
