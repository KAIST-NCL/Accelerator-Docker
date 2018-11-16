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

    //Status file + User defined device file --> From status file, parse status and pid of only those who are defined in user defined list
    for(list<Device>::iterator it = devs_usr.begin() ; it != devs_usr.end() ; it++){
        bool _continue = true;
        for(list<Device>::iterator it_stat = devs_stat.begin() ; _continue && it_stat != devs_stat.end() ; it_stat++){
            if(it->getName().compare(it_stat->getName()) == 0 ){
                _continue = false;
                it->setStatus(it_stat->getStatus());
                it->setPid(it_stat->getPid());
                // If pid is unavailable(not running), it is considered as available
                if(it->getStatus() == Device::Status::UNAVAILABLE && kill(it->getPid(),0) < 0){
                    it->setStatus(Device::Status::AVAILABLE);
                    it->setPid(0);
                }
            }
        }
    }

    *to = devs_usr;
    return isListValid(*to);
}

list<Device> DeviceParser::protoToDevice(device::device_list* proto_dev){
    list<Device> devs;
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

        // File path
        for(int i = 0; i < t_dev.file_size(); i++)
            dev.addFile(t_dev.file(i).src(),t_dev.file(i).dst());

        // Env var.
        for(int i = 0; i < t_dev.env_size(); i++)
            dev.addEnv(t_dev.env(i).key(),t_dev.env(i).val());

        // push back to device list
        devs.push_back(dev);
    }
    return devs;
}

bool DeviceParser::isListValid(list<Device> deviceList){
    bool result = true;
    for(list<Device>::iterator it = deviceList.begin() ; result && it != deviceList.end() ; it++){
        result = result && isDeviceValid(*it);
    }
    return result;
}

bool DeviceParser::isDeviceValid(Device device){
    //File existence check
    list<string> devs = device.getDevices();
    list<string> libs = device.getLibraries();
    list<array<string,2>> files = device.getFiles();

    list<string> files_arr;
    files_arr.insert(files_arr.end(),devs.begin(),devs.end());
    files_arr.insert(files_arr.end(),libs.begin(),libs.end());
    for(list<array<string,2>>::iterator it = files.begin(); it != files.end(); it++){
        files_arr.push_back(it->at(0));
        if(it->at(1).empty()){
            errx(1,"Destination for [%s] is not specified\n",it->at(0).c_str());
            return false;
        }
    }

    for(list<string>::iterator it = files_arr.begin(); it != files_arr.end(); it++){
        if( ! isFileExisting(it->c_str()) ){
            errx(1,"File [%s] not exists\n",it->c_str());
            return false;
        }
    }

    //PCI Slot check - TODO
    string slot = device.getPciSlot();
    uint16_t vendorID = device.getVendorID();
    uint16_t deviceID = device.getDeviceID();
    uint16_t subVendorID = device.getSubVendorID();
    uint16_t subDeviceID = device.getSubDeviceID();

    return true;
}