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
        bool detect(list<Accelerator>*);
        bool detect(list<Accelerator>*, list<Accelerator>);
    private:
};

class DeviceParser{
    public:
        DeviceParser(char*, char*);
        bool parse(list<Accelerator>*);
        bool parse(list<Accelerator>*,const char*);
    private:
        char* statusFilePath;
        char* deviceFilePath;

        list<Accelerator> protoToAccelerator(device::accelerator_list*);
        list<Device> protoToDevice(const device::device_list*);
        map<string,Device> devListToDevMap(list<Device>);
        bool isListValid(list<Accelerator>);
        bool isAcceleratorValid(Accelerator);
        bool isDeviceValid(Device);
};
#endif
