#include "acc-manager.hpp"
#include "cli.hpp"
#include "manager.hpp"
#include "mounter.hpp"

bool configure_parse(int argc, char** argv, Context* ctx)
{
    for (int i = 0; i < argc; i++) {
        cmatch cm;
        regex r_pid("--pid=([0-9]*)");
        regex r_dev("--device=(.*)");
        regex r_rfs("^--");
        if (regex_match(argv[i], cm, r_pid) && cm.size() > 1) {
            ctx->pid = atoi(cm[1].str().c_str());
        } else if (regex_match(argv[i], cm, r_dev) && cm.size() > 1) {
            char* tmp;
            char* target = (char*)cm[1].str().c_str();
            tmp = strtok(target, ",");
            if(tmp == NULL)
                ctx->devices_name.push_back(string("all"));
            while (tmp != NULL) {
                string dev(tmp);
                ctx->devices_name.push_back(dev);
                tmp = strtok(NULL, ",");
            }
        } else if (i == argc - 1 && !regex_match(argv[i], r_rfs)) {
            string arg(argv[i]);
            ctx->rootfs = arg;
        }
    }
    return ctx->validate();
}

bool configure_command(Context* ctx)
{
    Container container(0, 0, ctx->pid, ctx->rootfs);
    if(!container.parseOwner()){
        errx(ERR_CODE_PARSE_ERROR,"parsing container rootfs owner failed");
        return false;
    }

    Manager manager(ctx, &container);
    if (!manager.requestDevices()){
        manager.releaseDevices();
        errx(ERR_CODE_PARSE_ERROR,"requesting devices failed");
        return false;
    }

    Mounter mounter(container);
    if(!mounter.mountDevices(ctx->devices)){
        manager.releaseDevices();
        errx(ERR_CODE_PARSE_ERROR,"mounting devices failed");
        return false;
    }
    return true;
}