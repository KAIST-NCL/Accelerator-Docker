#include "common.hpp"
#include <iostream>
Context::Context(): pid(0){}
Context::Context(uid_t _uid, gid_t _gid) : uid(_uid),gid(_gid),pid(0){}
bool Context::validate(){
    char proc_dir[PATH_LEN_MAX];
    struct stat s_root, s_proc;
    int n;
    if(stat(rootfs.c_str(),&s_root) < 0 || rootfs.empty() || !S_ISDIR(s_root.st_mode)){
        errx(ERR_CODE_NO_ROOTFS,"rootfs [%s] does not exist",rootfs.c_str());
        return false;
    }
    if(pid <= 0 || kill(pid,0) < 0){
        errx(ERR_CODE_PID_NOT_VALID,"pid [%d] not valid",pid);
        return false;
    }
    if( (n = snprintf(proc_dir,sizeof(proc_dir),"/proc/%d",pid)) <0 || n >= PATH_LEN_MAX){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }
    if(stat(proc_dir,&s_proc) < 0 || !S_ISDIR(s_proc.st_mode)){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }
    return true;
}

Container::Container(uid_t _uid, gid_t _gid, pid_t _pid, string _rootfs) : uid(_uid), gid(_gid), pid(_pid), rootfs(_rootfs){}
pid_t Container::getPid(){ return pid; }
string Container::getRootFs(){ return rootfs; }
bool Container::parseOwner(){
    char proc_dir[PATH_LEN_MAX];
    struct stat s_proc;
    int n;
    if( (n = snprintf(proc_dir,sizeof(proc_dir),"/proc/%d",pid)) < 0 || n >= PATH_LEN_MAX ){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }
    if(stat(proc_dir,&s_proc) < 0 || !S_ISDIR(s_proc.st_mode)){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }
    uid = s_proc.st_uid;
    gid = s_proc.st_gid;
    return true;
}

Device::Device(string _name) : name(_name){
    status = Status::AVAILABLE;
    pid = 0;
}
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
Device::Status Device::getStatus(){
    return status;
}
void Device::setStatus(Device::Status stat){
    status = stat;
}
pid_t Device::getPid(){
    return pid;
}
void Device::setPid(pid_t _pid){
    pid = _pid;
}
string Device::getPciSlot(){
    return pciSlot;
}
void Device::setPciSlot(string slot){
    pciSlot = slot;
}
uint16_t Device::getVendorID(){
    return vendorID;
}
uint16_t Device::getDeviceID(){
    return deviceID;
}
uint16_t Device::getSubVendorID(){
    return subVendorID;
}
uint16_t Device::getSubDeviceID(){
    return subDeviceID;
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