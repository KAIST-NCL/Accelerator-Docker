#include "cli.hpp"
#include "driver-manager.hpp"

// Driver Manager
// : Handles device driver switch --> not implemented yet
DriverManager::DriverManager(){}
bool DriverManager::switchDriver(Device device, Driver target){
    return true;
}
list<Device> DriverManager::getAllDevices(){
    return devices;
}
list<Driver> DriverManager::getAllDrivers(){
    return drivers;
}

bool driver_parse(int argc, char** argv, Context *ctx){

}

bool driver_command(Context *ctx){

}