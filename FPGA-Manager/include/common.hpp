#ifndef _ACC_MANAGER_COMMON
#define _ACC_MANAGER_COMMON

#include <cstdint>
#include <cstring>
#include <string>
#include <list>
#include <iostream>
#include <cinttypes>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>

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

#define USR_DEF_DEV     "/etc/fpga-docker/devices.json"
#define STATUS_CFG      "/etc/fpga-docker/stat.json"

#define ERR_CODE_NO_ROOTFS 21
#define ERR_CODE_NO_PROC_DIR 22
#define ERR_CODE_PID_NOT_VALID 23

using namespace std;
//
class Device;
class LibraryNode;

class Context {
    public:
        Context();
        Context(uid_t,gid_t);
        bool validate();
        /* main */
        uid_t uid;
        gid_t gid;

        /* configure */
        pid_t pid;
        string rootfs;
        list<string> devices_name;
        list<Device> devices;
};

class Container{
    public:
        Container(uid_t _uid, gid_t _gid, pid_t _pid, string _rootfs);
        pid_t getPid();
        string getRootFs();
        bool parseOwner();
    private:
        uid_t uid;
        gid_t gid;
        pid_t pid;
        string rootfs;
};
class Device{
    public:
        enum Status{
            AVAILABLE,
            UNAVAILABLE
        };
        Device(string _name);
        list<string> getDeviceFilePaths();
        list<LibraryNode> getLibraries();
        void addDeviceFilePath(string path);
        void addLibraryNode(LibraryNode lib);
        string getName();
        Device::Status getStatus();
        void setStatus(Device::Status);
        pid_t getPid();
        void setPid(pid_t);
        string getPciSlot();
        void setPciSlot(string);
        uint16_t getVendorID();
        uint16_t getDeviceID();
        uint16_t getSubVendorID();
        uint16_t getSubDeviceID();
    private:
        string name;
        Device::Status status;
        pid_t pid;
        list<string> devFilePaths;
        list<LibraryNode> libs;
        string pciSlot;
        uint16_t vendorID;
        uint16_t deviceID;
        uint16_t subVendorID;
        uint16_t subDeviceID;
};
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

class LibraryNode{
    public:
        LibraryNode(string _src, string _dst);
        string getSrc();
        string getDst();
    private:
        string src;
        string dst;
};

#endif
