#include "device-detector.hpp"

using namespace std;
using namespace google;
using namespace device;

DeviceDetector::DeviceDetector(){}

bool DeviceDetector::detect(list<Device>* to){
    list<Device> devs;
    //TODO : implement detect method
    *to = devs;
    return true;
}

bool DeviceDetector::detect(list<Device>* to, list<Device> _dev){
    list<Device> devs;
    //TODO : implement detect method
    _dev.insert(_dev.end(),devs.begin(),devs.end());
    *to = _dev;
    return true;
}

DeviceParser::DeviceParser(char* _statusFilePath, char* _deviceFilePath) : statusFilePath(_statusFilePath), deviceFilePath(_deviceFilePath){}

bool DeviceParser::parse(list<Device>* to){
    device::device_list *stat_list = new device::device_list();
    device::device_list *usr_list = new device::device_list();

    fstream fd1(statusFilePath,ios::in | ios::binary);
    stat_list->ParseFromIstream(&fd1);
    list<Device> devs_stat = protoToDevice(stat_list);

    int fd0 = open(deviceFilePath, O_RDONLY);
    protobuf::io::FileInputStream fileInput(fd0);
    protobuf::TextFormat::Parse(&fileInput, usr_list);
    close(fd0);
    list<Device> devs_usr = protoToDevice(usr_list);

    //Status file + User defined device file --> User defined에 있는 애만 status file에서 status 등 파싱
    for(list<Device>::iterator it = devs_usr.begin() ; it != devs_usr.end() ; it++){
        bool _continue = true;
        for(list<Device>::iterator it_stat = devs_stat.begin() ; _continue && it_stat != devs_stat.end() ; it_stat++){
            if(it->getName().compare(it_stat->getName()) == 0 ){
                _continue = false;
                it->setStatus(it_stat->getStatus());
                it->setPid(it_stat->getPid());
            }
        }
    }

    *to = devs_usr;
    return true;
}

list<Device> DeviceParser::protoToDevice(device::device_list* proto_dev){
    list<Device> devs;
    // TODO : Update is required 
    // Devie class inherits device class defined by protocolbuffer
    for(int idx = 0; idx < proto_dev->devices_size(); idx ++){
        device::device t_dev = proto_dev->devices(idx);
        Device dev(t_dev.name());
        if(t_dev.has_type())
            dev.setType(t_dev.type());
        if(t_dev.has_pci())
            dev.setPciSlot(t_dev.pci());
        if(t_dev.has_status())
            dev.setStatus(t_dev.status() == device::device_Status::device_Status_IDLE ? Device::Status::AVAILABLE : Device::Status::UNAVAILABLE);
        if(t_dev.has_pid())
            dev.setPid(t_dev.pid());

        // Device driver file path
        for(int i = 0; i < t_dev.device_driver_size(); i++)
            dev.addDevice(t_dev.device_driver(i));

        // Library path
        for(int i = 0; i < t_dev.library_size(); i++)
            dev.addLibrary(t_dev.library(i));

        // push back to device list
        devs.push_back(dev);
    }
    return devs;
}