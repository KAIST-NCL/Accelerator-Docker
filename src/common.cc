#include "common.hpp"

Container::Container(pid_t _pid, string _rootfs) : pid(_pid), rootfs(_rootfs){}
pid_t Container::getPid(){ return pid; }
string Container::getRootFs(){ return rootfs; }

Device::Device(string _name) : name(_name){}
list<string> Device::getDeviceFilePaths(){ return devFilePaths; }
list<LibraryNode> Device::getLibraries(){ return libs; }

void Device::addDeviceFilePath(string path){
    devFilePaths.push_back(path);
}
void Device::addLibraryNode(LibraryNode lib){
    libs.push_back(lib);
}

string Device::getName(){
    return name;
}

string Device::getPciSlot(){
    return pciSlot;
}

Driver::Driver(string _name, int _vendorID, int _deviceID, int _subVendorID, int _subDeviceID)
 : name(_name), vendorID(_vendorID), deviceID(_deviceID), subVendorID(_subVendorID), subDeviceID(_subDeviceID){}
list<string> Driver::getModules(){ return modules; }


void Driver::addModuleNode(string mod){
    modules.push_back(mod);
}
int Driver::getVendorID(){
    return vendorID;
}
int Driver::getDeviceID(){
    return deviceID;
}
int Driver::getSubVendorID(){
    return subVendorID;
}
int Driver::getSubDeviceID(){
    return subDeviceID;
}

LibraryNode::LibraryNode(string _src, string _dst)
 : src(_src), dst(_dst){}
string LibraryNode::getSrc(){
    return src;
}
string LibraryNode::getDst(){
    return dst;
}
