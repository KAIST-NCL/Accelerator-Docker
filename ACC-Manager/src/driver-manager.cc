#include "cli.hpp"
#include "driver-manager.hpp"

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