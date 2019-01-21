#include "device-parser.hpp"

using namespace std;
using namespace google;

// Device Parser
// : Parse user-specified devices in /etc/accelerator-docker/{device.pbtxt,stat.pb}
DeviceParser::DeviceParser(char* _statusFilePath, char* _deviceFilePath) : statusFilePath(_statusFilePath), deviceFilePath(_deviceFilePath){}

// Parse device list from /etc/accelerator-docker/device.pbtxt and move it to 'to'
bool DeviceParser::parse(list<Accelerator>* to){
    auto *usr_list = new device::accelerator_list();
    auto *stat_list = new device::device_list();

    // Status file : stat.pb (binary)
    fstream fd1(statusFilePath,ios::in | ios::binary);
    stat_list->ParseFromIstream(&fd1);
    map<string,Device> devs_stat = devListToDevMap(protoToDeviceList(stat_list));

    // User-specified devices list : device.pbtxt (text form)
    int fd0 = open(deviceFilePath, O_RDONLY);
    protobuf::io::FileInputStream fileInput(fd0);
    protobuf::TextFormat::Parse(&fileInput, usr_list);
    close(fd0);
    list<Accelerator> accs_usr = protoToAccelerator(usr_list);

    //Status file + User defined device file --> From status file, parse status and pid of only those who are defined in user defined list
    //if no id set to some device, set it here
    for(auto & it_acc : accs_usr){
        list<Device> devs = it_acc.getDevices();
        // For each device instance of an accelerator type, find if status is saved in stat.pb
        for(auto & it_dev : devs){
            string dev_id = generateDeviceId(it_dev);
            it_dev.setId(dev_id);
            auto dev_stat = devs_stat.find(dev_id);
            if(dev_stat != devs_stat.end()){
                it_dev.setId(dev_stat->second.getId());
                it_dev.setStatus(dev_stat->second.getStatus());
                it_dev.setPid(dev_stat->second.getPid());
                // If pid is unavailable(not running), it is considered as available
                struct stat s = {};
                string proc_str = "/proc/"+to_string(it_dev.getPid());
                const char* proc_str_c = proc_str.c_str();
                stat(proc_str_c, &s);
                if(it_dev.getStatus() == Device::Status::UNAVAILABLE && !S_ISDIR(s.st_mode)){
                    it_dev.setStatus(Device::Status::AVAILABLE);
                    it_dev.setPid(0);
                }
                // Validity check and set as MISCONFIGURED status if something is not configured correctly
                if(!isDeviceValid(it_dev))
                    it_dev.setStatus(Device::Status::MISCONFIGURED);
            }
        }
        it_acc.setDevices(devs);
    }

    *to = accs_usr;
    return isListValid(*to);
}

// From protobuf type 'accelerator_list' to list of Accelerator class
list<Accelerator> DeviceParser::protoToAccelerator(device::accelerator_list* proto_acc){
    list<Accelerator> accs;
    for(int idx = 0; idx < proto_acc->accelerators_size(); idx++){
        const device::accelerator & t_acc = proto_acc->accelerators(idx);
        Accelerator acc(t_acc.type());
        list<Device> devs;
        for(int i = 0; i < t_acc.devices_size() ; i ++){
            const device::device & t_dev = t_acc.devices(i);
            devs.push_back(protoToDevice(&t_dev));
        }
        acc.setDevices(devs);
        accs.push_back(acc);
    }
    return accs;
}

// From protobuf type 'device_list' to list of Device class
list<Device> DeviceParser::protoToDeviceList(const device::device_list* proto_dev){
    list<Device> devs;
    // Devie class inherits device class defined by protocolbuffer
    for(int idx = 0; idx < proto_dev->devices_size(); idx ++){
        const device::device & t_dev = proto_dev->devices(idx);
        Device dev = protoToDevice(&t_dev);
        devs.push_back(dev);
    }
    return devs;
}

// From protobuf type 'device_list' to list of Device class
Device DeviceParser::protoToDevice(const device::device* t_dev){
    Device dev(t_dev->name());
    if(t_dev->has_pci())
        dev.setPciSlot(t_dev->pci());
    if(t_dev->has_status())
        dev.setStatus(t_dev->status() == device::device_Status::device_Status_IDLE ? Device::Status::AVAILABLE : Device::Status::UNAVAILABLE);
    if(t_dev->has_pid())
        dev.setPid(t_dev->pid());
    if(t_dev->has_id())
        dev.setId(t_dev->id());

    // Device driver file path
    for(int i = 0; i < t_dev->device_file_size(); i++)
        dev.addDeviceFile(t_dev->device_file(i));

    // Library path
    for(int i = 0; i < t_dev->library_size(); i++)
        dev.addLibrary(t_dev->library(i));

    // File path
    for(int i = 0; i < t_dev->file_size(); i++)
        dev.addFile(t_dev->file(i).src(),t_dev->file(i).dst());

    // Env var.
    for(int i = 0; i < t_dev->env_size(); i++)
        dev.addEnv(t_dev->env(i).key(),t_dev->env(i).val());
    return dev;
}

// Convert from list to map easy to find by id
map<string,Device> DeviceParser::devListToDevMap(list<Device> devList){
    map<string,Device> devMap;

    for(auto it : devList){
        devMap.insert(pair<string,Device>(it.getId(),it));
    }

    return devMap;
}

// Check if accelerator list is valid
bool DeviceParser::isListValid(list<Accelerator> accList){
    // TODO: Check validity of accelerator type and device name
    for(auto const & it : accList){
        bool tmp = isAcceleratorValid(it);
        if(!tmp)
            return false;
    }
    return true;
}

// Check if an Accelerator class valid
bool DeviceParser::isAcceleratorValid(Accelerator acc){
    list<Device> deviceList = acc.getDevices();
    for(auto const & it : deviceList){
        bool tmp = isDeviceValid(it);
        if(!tmp)
            return false;
    }
    return true;
}

// Check if a Device class valid
bool DeviceParser::isDeviceValid(Device device){
    list<string> devs = device.getDeviceFiles();
    list<string> libs = device.getLibraries();
    list<array<string,2>> files = device.getFiles();

    // Append device file, library file, file list together
    list<string> files_arr;
    files_arr.insert(files_arr.end(),devs.begin(),devs.end());
    files_arr.insert(files_arr.end(),libs.begin(),libs.end());

    // Check validity of Src, Dst set
    for(auto it : files){
        files_arr.push_back(it.at(0));
        if(it.at(1).empty()){
            //errx(1,"Destination for [%s] is not specified\n",it.at(0).c_str());
            return false;
        }
    }

    // Check if the files exist
    for(auto const & it : files_arr){
        if( ! isFileExisting(it.c_str()) ){
            //errx(1,"File [%s] not exists\n",it->c_str());
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