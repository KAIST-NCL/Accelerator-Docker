#include "mounter.hpp"

Mounter::Mounter(Container _cont) : cont(_cont){}
bool Mounter::mountDevice(Device device){
    bool result = true;
    list<string> dev_files = device.getDeviceFilePaths();
    list<LibraryNode> lib_files = device.getLibraries();
    for(list<string>::iterator it = dev_files.begin() ; result && it != dev_files.end(); it++){
        result = result && mountDeviceFile(*it);
    }
    for(list<LibraryNode>::iterator it = lib_files.begin() ; result && it != lib_files.end(); it++){
        result = result && mountLibraryFile(*it);
    }
    return result;
}
bool Mounter::mountDeviceFile(string dev){
    return true;
}
bool Mounter::mountLibraryFile(LibraryNode lib){
    return true;
}