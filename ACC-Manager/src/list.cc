#include "cli.hpp"
#include "manager.hpp"

// Nothing to do with parsing in list command
bool list_parse(int argc, char** argv, Context *ctx){
    return true;
}

// List command current status of each accelerator devices
bool list_command(Context *ctx){
    Manager manager(ctx);
    list<Accelerator> accs = manager.getAcceleratorList();

    printf("+------------+------------------------+------------------------------+----------------+----------------+----------+\n");
    printf("|     ID     |          Name          |             Type             |    PCI-Slot    |     Status     |  Holder  |\n");
    printf("+------------+------------------------+------------------------------+----------------+----------------+----------+\n");
    for(auto it_acc : accs){
        list<Device> devices = it_acc.getDevices();
        for(auto it : devices){
            printf("| %-11s| %-23s| %-29s| %-15s| %-15s| %-9d|\n",
                it.getId().c_str(),
                it.getName().c_str(),
                it_acc.getType().c_str(),
                it.getPciSlot().c_str(),
                it.getStatus() == Device::Status::AVAILABLE ? "Available" : it.getStatus() == Device::Status::MISCONFIGURED ? "Misconfigured" : "Unavailable",
                it.getPid()
            );
        }
    }
    printf("+------------+------------------------+------------------------------+----------------+----------------+----------+\n");
    return true;
}