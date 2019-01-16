#include "common.hpp"
#include <iostream>
Context::Context(uid_t _uid, gid_t _gid) : uid(_uid),gid(_gid),pid(0),rootfs(string("")){}
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
bool Context::parseOwner(){
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

uid_t Context::getUid(){ return uid; }
gid_t Context::getGid(){ return gid; }
pid_t Context::getPid(){ return pid; }
string Context::getRootFs(){ return rootfs; }
list<string> Context::getReqDevices(){ return req_devices_name; }
char* Context::getDeviceFilePath(){ return device_file_path; }
char* Context::getStatusFilePath(){ return status_file_path; }

void Context::setPid(pid_t _pid){pid=_pid;}
void Context::setRootFs(string _rootfs){rootfs=_rootfs;}
void Context::setDeviceFilePath(char* path){device_file_path=path;}
void Context::setStatusFilePath(char* path){status_file_path=path;}
void Context::addReqDevice(string dev){req_devices_name.push_back(dev);}

Accelerator::Accelerator(string _type): type(_type){

}
string Accelerator::getType(){ return type; }
list<Device> Accelerator::getDevices(){ return devices; }
void Accelerator::setDevices(list<Device> _devs){
    devices = _devs;
}
void Accelerator::addDevice(Device _device){
    devices.push_back(_device);
}

Device::Device(string _name): name(_name){
    status = Status::AVAILABLE;
    pid = 0;
}
list<string> Device::getDeviceFiles(){ return devs; }
list<string> Device::getLibraries(){ return libs; }
list<array<string,2>> Device::getFiles(){ return files; }
list<array<string,2>> Device::getEnvs(){ return envs; }
void Device::addDeviceFile(string path){
    devs.push_back(path);
}
void Device::addLibrary(string lib){
    libs.push_back(lib);
}
void Device::addFile(string src, string dst){
    files.push_back({src, dst});
}
void Device::addEnv(string key, string val){
    envs.push_back({key, val});
}
Device::Status Device::getStatus(){
    return status;
}
void Device::setStatus(Device::Status stat){
    status = stat;
}
string Device::getName(){
    return name;
}
void Device::setName(string _name){
    name = _name;
}
string Device::getId(){
    return id;
}
void Device::setId(string _id){
    id = _id;
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