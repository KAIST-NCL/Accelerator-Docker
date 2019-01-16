#include "device-detector.hpp"

using namespace std;
using namespace google;
using namespace device;

DeviceDetector::DeviceDetector(){}

bool DeviceDetector::detect(list<Accelerator>* to){
    list<Accelerator> accs;
    //TODO : implement detect method
    *to = accs;
    return true;
}

bool DeviceDetector::detect(list<Accelerator>* to, list<Accelerator> _acc){
    list<Accelerator> accs;
    //TODO : implement detect method
    _acc.insert(_acc.end(),accs.begin(),accs.end());
    *to = _acc;
    return true;
}

DeviceParser::DeviceParser(char* _statusFilePath, char* _deviceFilePath) : statusFilePath(_statusFilePath), deviceFilePath(_deviceFilePath){}

bool DeviceParser::parse(list<Accelerator>* to){
    device::accelerator_list *usr_list = new device::accelerator_list();
    device::device_list *stat_list = new device::device_list();

    fstream fd1(statusFilePath,ios::in | ios::binary);
    stat_list->ParseFromIstream(&fd1);
    map<string,Device> devs_stat = devListToDevMap(protoToDevice(stat_list));

    int fd0 = open(deviceFilePath, O_RDONLY);
    protobuf::io::FileInputStream fileInput(fd0);
    protobuf::TextFormat::Parse(&fileInput, usr_list);
    close(fd0);
    list<Accelerator> accs_usr = protoToAccelerator(usr_list);

    //Status file + User defined device file --> From status file, parse status and pid of only those who are defined in user defined list
    //if no id set to some device, set it here
    for(list<Accelerator>::iterator it_acc = accs_usr.begin(); it_acc != accs_usr.end(); it_acc ++){
        list<Device> devs = it_acc->getDevices();
        for(list<Device>::iterator it_dev = devs.begin() ; it_dev != devs.end() ; it_dev++){
            string dev_id = generateDeviceId(*it_dev);
            it_dev->setId(dev_id);
            map<string,Device>::iterator dev_stat = devs_stat.find(dev_id);
            if(dev_stat != devs_stat.end()){
                it_dev->setId(dev_stat->second.getId());
                it_dev->setStatus(dev_stat->second.getStatus());
                it_dev->setPid(dev_stat->second.getPid());
                // If pid is unavailable(not running), it is considered as available
                struct stat s;
                string proc_str = "/proc/"+to_string(it_dev->getPid());
                const char* proc_str_c = proc_str.c_str();
                stat(proc_str_c, &s);
                if(it_dev->getStatus() == Device::Status::UNAVAILABLE && !S_ISDIR(s.st_mode)){
                    it_dev->setStatus(Device::Status::AVAILABLE);
                    it_dev->setPid(0);
                }
            }
        }
        it_acc->setDevices(devs);
    }

    *to = accs_usr;
    return isListValid(*to);
}

list<Accelerator> DeviceParser::protoToAccelerator(device::accelerator_list* proto_acc){
    list<Accelerator> accs;
    for(int idx = 0; idx < proto_acc->accelerators_size(); idx++){
        device::accelerator t_acc = proto_acc->accelerators(idx);
        Accelerator acc(t_acc.type());
        if(t_acc.has_devices())
            acc.setDevices(protoToDevice(&t_acc.devices()));
        accs.push_back(acc);
    }
    return accs;
}

list<Device> DeviceParser::protoToDevice(const device::device_list* proto_dev){
    list<Device> devs;
    // Devie class inherits device class defined by protocolbuffer
    for(int idx = 0; idx < proto_dev->devices_size(); idx ++){
        device::device t_dev = proto_dev->devices(idx);
        Device dev(t_dev.name());
        if(t_dev.has_pci())
            dev.setPciSlot(t_dev.pci());
        if(t_dev.has_status())
            dev.setStatus(t_dev.status() == device::device_Status::device_Status_IDLE ? Device::Status::AVAILABLE : Device::Status::UNAVAILABLE);
        if(t_dev.has_pid())
            dev.setPid(t_dev.pid());
        if(t_dev.has_id())
            dev.setId(t_dev.id());

        // Device driver file path
        for(int i = 0; i < t_dev.device_file_size(); i++)
            dev.addDeviceFile(t_dev.device_file(i));

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

map<string,Device> DeviceParser::devListToDevMap(list<Device> devList){
    map<string,Device> devMap;

    for(list<Device>::iterator it = devList.begin(); it != devList.end(); it++){
        devMap.insert(pair<string,Device>(it->getId(),*it));
    }

    return devMap;
}

bool DeviceParser::isListValid(list<Accelerator> accList){
    // TODO: Check validity of accelerator type and device name
    for(list<Accelerator>::iterator it = accList.begin(); it != accList.end(); it++){
        bool tmp = isAcceleratorValid(*it);
        if(!tmp)
            return false;
    }
    return true;
}

bool DeviceParser::isAcceleratorValid(Accelerator acc){
    list<Device> deviceList = acc.getDevices();
    for(list<Device>::iterator it = deviceList.begin() ; it != deviceList.end() ; it++){
        bool tmp = isDeviceValid(*it);
        if(!tmp)
            return false;
    }
    return true;
}

bool DeviceParser::isDeviceValid(Device device){
    //File existence check
    list<string> devs = device.getDeviceFiles();
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