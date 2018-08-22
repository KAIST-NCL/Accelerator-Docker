#ifndef _ACC_MANAGER_DEVICE_DETECTOR
#define _ACC_MANAGER_DEVICE_DETECTOR

#include "common.hpp"

class DeviceDetector{
    public:
        DeviceDetector();
        list<Device> detect();
    private:
};

class DeviceParser{
    public:
        DeviceParser(string _configFilePath);
        list<Device> parse();
    private:
        string configFilePath;
        bool isListValid(list<Device> deviceList);
        bool isDeviceValid(Device device);
};

#endif