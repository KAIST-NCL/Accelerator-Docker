#ifndef _ACC_MANAGER_DEVICE_DETECTOR
#define _ACC_MANAGER_DEVICE_DETECTOR

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <pci/pci.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "device.pb.h"
#include "common.hpp"
#include "utils.hpp"

// Device auto-detector --> not implemented yet
class DeviceDetector{
    public:
        DeviceDetector();
        bool detect(list<Accelerator>*);
        bool detect(list<Accelerator>*, list<Accelerator>);
    private:
};
#endif
