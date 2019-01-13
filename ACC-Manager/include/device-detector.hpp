#ifndef _ACC_MANAGER_DEVICE_DETECTOR
#define _ACC_MANAGER_DEVICE_DETECTOR

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string>
//#include <pci/pci.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "device.pb.h"
#include "common.hpp"
#include "utils.hpp"

class DeviceDetector{
    public:
        DeviceDetector();
        bool detect(list<Device>*);
        bool detect(list<Device>*, list<Device>);
    private:
};

class DeviceParser{
    public:
        DeviceParser(char*, char*);
        bool parse(list<Device>*);
        bool parse(list<Device>*,const char*);
    private:
        char* statusFilePath;
        char* deviceFilePath;

        list<Device> protoToDevice(device::device_list*);
        bool isListValid(list<Device> deviceList);
        bool isDeviceValid(Device device);
};
#endif
