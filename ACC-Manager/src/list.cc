#include "cli.hpp"
#include "manager.hpp"

bool list_parse(int argc, char** argv, Context *ctx){
    return true;
}
bool list_command(Context *ctx){
    Manager manager(ctx);
    list<Device> devices = manager.getDeviceList();

    printf("+------+--------------------+----------+----------------+----------------+----------+\n");
    printf("|  ID  |        Name        |   Type   |    PCI-Slot    |     Status     |  Holder  |\n");
    printf("+------+--------------------+----------+----------------+----------------+----------+\n");
    int i = 0;
    for(list<Device>::iterator it = devices.begin(); it != devices.end(); it++){
        printf("| %-5d| %-19s| %-9s| %-15s| %-15s| %-9d|\n",
            i++,
            it->getName().c_str(),
            it->getType().c_str(),
            it->getPciSlot().c_str(),
            it->getStatus() == Device::Status::AVAILABLE ? "Available" : "Unavailable",
            it->getPid()
        );
    }
    printf("+------+--------------------+----------+----------------+----------------+----------+\n");
    return true;
}