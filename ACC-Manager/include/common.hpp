#ifndef _ACC_MANAGER_COMMON
#define _ACC_MANAGER_COMMON

#include <cstdint>
#include <cstring>
#include <string>
#include <list>
#include <map>
#include <functional>
#include <unordered_map>
#include <array>
#include <iostream>
#include <cinttypes>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>

#define PATH_LEN_MAX 1024

#define USR_BIN_DIR               "/usr/bin"
#define LIB_DIR                   "/lib64"
#define USR_LIB_DIR               "/usr/lib64"
#define USR_LIB32_DIR             "/usr/lib32"
#define USR_LIB32_ALT_DIR         "/usr/lib"
#if defined(__x86_64__)
    #define USR_LIB_MULTIARCH_DIR    "/usr/lib/x86_64-linux-gnu"
    #define USR_LIB32_MULTIARCH_DIR  "/usr/lib/i386-linux-gnu"
    #if !defined(__NR_execveat)
        #define __NR_execveat 322
    #endif
#elif defined(__powerpc64__)
    #define USR_LIB_MULTIARCH_DIR    "/usr/lib/powerpc64le-linux-gnu"
    #define USR_LIB32_MULTIARCH_DIR  "/var/empty"
    #if !defined(__NR_execveat)
        #define __NR_execveat 362
    # endif
#else
    # error "unsupported architecture"
#endif

#define USR_DEF_DEV     "/etc/accelerator-docker/device.pbtxt"
#define STATUS_CFG      "/etc/accelerator-docker/stat.pb"

#define ERR_CODE_NO_ROOTFS 21
#define ERR_CODE_NO_PROC_DIR 22
#define ERR_CODE_PID_NOT_VALID 23
#define ERR_CODE_PROCESS_STILL_RUNNING 24

using namespace std;

class Device;

// Context class for storing container info.
class Context {
    public:
        Context(uid_t,gid_t);
        bool validate();
        bool parseOwner();

        uid_t getUid();
        gid_t getGid();
        pid_t getPid();
        string getRootFs();
        list<string> getReqDevices();
        char* getDeviceFilePath();
        char* getStatusFilePath();

        void setPid(pid_t);
        void setRootFs(string);
        void setDeviceFilePath(char*);
        void setStatusFilePath(char*);
        void addReqDevice(string);
    private:
        uid_t uid;
        gid_t gid;
        pid_t pid;
        string rootfs;

        char* device_file_path;
        char* status_file_path;

        list<string> req_devices_name;
};

// Accelerator container, refer to device.proto
class Accelerator{
    public:
        explicit Accelerator(string);
        string getType();
        list<Device> getDevices();
        void setDevices(list<Device>);
        void addDevice(Device);
    private:
        string type;
        list<Device> devices;
};

// Device container, refer to device.proto
class Device{
    public:
        enum Status{
            AVAILABLE,
            UNAVAILABLE,
            MISCONFIGURED
        };
        explicit Device(string);

        list<string> getDeviceFiles();
        list<string> getLibraries();
        list<array<string,2>> getFiles();
        list<array<string,2>> getEnvs();
        Device::Status getStatus();
        string getName();
        string getId();
        pid_t getPid();
        string getPciSlot();
        uint16_t getVendorID();
        uint16_t getDeviceID();
        uint16_t getSubVendorID();
        uint16_t getSubDeviceID();
        void setName(string);
        void setId(string);
        void setType(string);
        void setStatus(Device::Status);
        void setPid(pid_t);
        void setPciSlot(string);
        void addDeviceFile(string);
        void addLibrary(string);
        void addFile(string,string);
        void addEnv(string,string);
    private:
        Device::Status status;
        string name;
        string id;
        pid_t pid;
        list<string> devs;              // device files
        list<string> libs;              // library files
        list<array<string,2>> files;    // files
        list<array<string,2>> envs;     // env vars
        string pciSlot;
        uint16_t vendorID;
        uint16_t deviceID;
        uint16_t subVendorID;
        uint16_t subDeviceID;
};

// Class for auto-detect --> not implemented yet
class Driver{
    public:
        Driver(string _name, int _vendorID, int _deviceID, int _subVendorID, int _subDeviceID);
        list<string> getModules();
        void addModuleNode(string mod);

        int getVendorID();
        int getDeviceID();
        int getSubVendorID();
        int getSubDeviceID();
    private:
        string name;
        list<string> modules;
        //For Matching Devices
        int vendorID;
        int deviceID;
        int subVendorID;
        int subDeviceID;
};

#endif
