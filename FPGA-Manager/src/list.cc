#include "cli.hpp"
#include "manager.hpp"

bool list_parse(int argc, char** argv, Context *ctx){
    return true;
}
bool list_command(Context *ctx){
    Manager manager(ctx,(Container*)0);
    list<Device> devices;
    if(!manager.parseAndDetectDevices(&devices)){
        errx(ERR_CODE_PARSE_DETECT_DEVICES_FAILED,"detect and parsing devices failed");
        return false;
    }

    printf("+----------+--------------------+----------------+----------------------+--------------+\n");
    printf("|    ID    |        Name        |    PCI-Slot    |        Status        |    Holder    |\n");
    printf("+----------+--------------------+----------------+----------------------+--------------+\n");
    int i = 0;
    for(list<Device>::iterator it = devices.begin(); it != devices.end(); it++){
        printf("| %-9d| %-19s| %-15s| %-21s| %-13d|\n",
            i++,
            it->getName().c_str(),
            it->getPciSlot().c_str(),
            it->getStatus() == Device::Status::AVAILABLE ? "Available" : "Unavailable",
            it->getPid()
        );
    }
    printf("+----------+--------------------+----------------+----------------------+--------------+\n");
    return true;
}