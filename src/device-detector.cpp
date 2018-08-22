#include "device-detector.hpp"

DeviceDetector::DeviceDetector(){}
list<Device> DeviceDetector::detect(){
    list<Device> devs;
    return devs;
}

DeviceParser::DeviceParser(string _configFilePath) : configFilePath(_configFilePath){}
list<Device> DeviceParser::parse(){
    list<Device> devs;
    return devs;
}