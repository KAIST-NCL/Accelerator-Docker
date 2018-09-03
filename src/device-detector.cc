#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "device-detector.hpp"
#include "device.pb.h"
#include "common.hpp"

using namespace std;
using namespace google;
using namespace device;

DeviceDetector::DeviceDetector(){}

list<Device> DeviceDetector::detect(){
    list<Device> devs;
    return devs;
}

DeviceParser::DeviceParser(char* _configFilePath) : configFilePath(_configFilePath){}

list<Device> DeviceParser::parse(){return parse(configFilePath);}

list<Device> DeviceParser::parse(const char* _configFilePath){

    list<Device> devs;

    device::device_list *dev_list = new device::device_list();
    int fd = open(configFilePath, O_RDONLY);
    protobuf::io::FileInputStream fileInput(fd);
    protobuf::TextFormat::Parse(&fileInput, dev_list);

    // Update is required 
    // Devie class inherits device class defined by protocolbuffer
    for(int idx = 0; idx < dev_list->devices_size(); idx ++){
        device::device t_dev = dev_list->devices(idx);
        Device dev(t_dev.name());

        // Device driver file path
        for(int i = 0; i < t_dev.device_driver_size(); i++)
            dev.addDeviceFilePath(t_dev.device_driver(i));

        // Library path
        for(int i = 0; i < t_dev.library_size(); i++)
            dev.addLibraryNode(LibraryNode(t_dev.library(i), t_dev.library(i)));

        // push back to device list
        devs.push_back(dev);
    }


    return devs;
}
