#ifndef _ACC_MANAGER_COMMON
#define _ACC_MANAGER_COMMON

#include <cstdint>
#include <cstring>
#include <string>
#include <list>
#include <cinttypes>

using namespace std;
//
class LibraryNode;

class Container{
    public:
        Container(pid_t _pid, string _rootfs);
        pid_t getPid();
        string getRootFs();
    private:
        pid_t pid;
        string rootfs;
};
class Device{
    public:
        Device(string _name);
        list<string> getDeviceFilePaths();
        list<LibraryNode> getLibraries();
        void addDeviceFilePath(string path);
        void addLibraryNode(LibraryNode lib);
        string getPciSlot();
        uint16_t getVendorID();
        uint16_t getDeviceID();
        uint16_t getSubVendorID();
        uint16_t getSubDeviceID();
    private:
        string name;
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