#include "common.hpp"
#include <iostream>
Context::Context(uid_t _uid, gid_t _gid) : uid(_uid),gid(_gid),pid(0),rootfs(string("")){}

// Validate given container information
bool Context::validate(){
    char proc_dir[PATH_LEN_MAX];
    struct stat s_root={};
    struct stat s_proc={};
    int n;

    // Existence check for given rootfs
    if(stat(rootfs.c_str(),&s_root) < 0 || rootfs.empty() || !S_ISDIR(s_root.st_mode)){
        errx(ERR_CODE_NO_ROOTFS,"rootfs [%s] does not exist",rootfs.c_str());
        return false;
    }

    // Existence check for given pid
    // Only used for 'configure' right now, which always run as root --> no permission issue for 'kill'
    // May be deleted when permission issue occurs (pid existence check is done with /proc/{PID} check)
    if(pid <= 0 || kill(pid,0) < 0){
        errx(ERR_CODE_PID_NOT_VALID,"pid [%d] not valid",pid);
        return false;
    }

    // Error handling for snprintf : make /proc/{PID} string
    if( (n = snprintf(proc_dir,sizeof(proc_dir),"/proc/%d",pid)) <0 || n >= PATH_LEN_MAX){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }

    // Existence check of /proc/{PID} dir.
    if( (stat(proc_dir,&s_proc) < 0) || !S_ISDIR(s_proc.st_mode)){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }
    return true;
}

// Parse UID,GID of given container
bool Context::parseOwner(){
    char proc_dir[PATH_LEN_MAX];
    struct stat s_proc ={};
    int n;
    if( (n = snprintf(proc_dir,sizeof(proc_dir),"/proc/%d",pid)) < 0 || n >= PATH_LEN_MAX ){
        errx(ERR_CODE_NO_PROC_DIR,"proc dir [/proc/%d] does not exist",pid);
        return false;
    }
    if( (stat(proc_dir,&s_proc) < 0) || !S_ISDIR(s_proc.st_mode)){
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

void Context::setPid(const pid_t _pid){pid=_pid;}
void Context::setRootFs(const string _rootfs){rootfs=_rootfs;}
void Context::setDeviceFilePath(char* path){device_file_path=path;}
void Context::setStatusFilePath(char* path){status_file_path=path;}
void Context::addReqDevice(const string dev){req_devices_name.push_back(dev);}


Accelerator::Accelerator(const string _type): type(_type){}
string Accelerator::getType(){ return type; }
list<Device> Accelerator::getDevices(){ return devices; }
void Accelerator::setDevices(const list<Device> _devs){
    devices = _devs;
}
void Accelerator::addDevice(const Device _device){
    devices.push_back(_device);
}


Device::Device(const string _name): name(_name){
    status = Status::AVAILABLE;
    pid = 0;
}
list<string> Device::getDeviceFiles(){ return devs; }
list<string> Device::getLibraries(){ return libs; }
list<array<string,2>> Device::getFiles(){ return files; }
list<array<string,2>> Device::getEnvs(){ return envs; }
Device::Status Device::getStatus(){ return status; }
string Device::getName(){ return name; }
string Device::getId(){ return id; }
pid_t Device::getPid(){ return pid; }
string Device::getPciSlot(){ return pciSlot; }
uint16_t Device::getVendorID(){ return vendorID; }
uint16_t Device::getDeviceID(){ return deviceID; }
uint16_t Device::getSubVendorID(){ return subVendorID; }
uint16_t Device::getSubDeviceID(){ return subDeviceID; }

void Device::addDeviceFile(const string path){
    devs.push_back(path);
}
void Device::addLibrary(const string lib){
    libs.push_back(lib);
}
void Device::addFile(const string src, const string dst){
    files.push_back({src, dst});
}
void Device::addEnv(const string key, const string val){
    envs.push_back({key, val});
}
void Device::setStatus(const Device::Status stat){
    status = stat;
}
void Device::setName(const string _name){
    name = _name;
}
void Device::setId(const string _id){
    id = _id;
}
void Device::setPid(const pid_t _pid){
    pid = _pid;
}
void Device::setPciSlot(const string slot){
    pciSlot = slot;
}

Driver::Driver(const string _name, const int _vendorID, const int _deviceID, const int _subVendorID, const int _subDeviceID)
 : name(_name), vendorID(_vendorID), deviceID(_deviceID), subVendorID(_subVendorID), subDeviceID(_subDeviceID){}
list<string> Driver::getModules(){ return modules; }
int Driver::getVendorID(){ return vendorID; }
int Driver::getDeviceID(){ return deviceID; }
int Driver::getSubVendorID(){ return subVendorID; }
int Driver::getSubDeviceID(){ return subDeviceID; }
void Driver::addModuleNode(string mod){
    modules.push_back(mod);
}