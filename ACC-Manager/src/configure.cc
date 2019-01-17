#include "cli.hpp"
#include "manager.hpp"
#include "mounter.hpp"

// Configure format
//  : acc-manager configure --pid=<PID> --device=<DEVICES> <ROOTFS>
bool configure_parse(int argc, char** argv, Context* ctx){
    for (int i = 0; i < argc; i++) {
        cmatch cm;
        regex r_pid("--pid=([0-9]*)");  // Parse PID
        regex r_dev("--device=(.*)");   // Parse required device
        regex r_rfs("^--");

        // PID arg
        if (regex_match(argv[i], cm, r_pid) && cm.size() > 1) {
            ctx->setPid( atoi(cm[1].str().c_str()) );
        }
        // DEVICE arg
        // Required devices are joined with comma(,) --> divide by comma and put it in required device list
        else if (regex_match(argv[i], cm, r_dev) && cm.size() > 1) {
            string arg = cm[1].str();
            string delimiter = ",";
            size_t pos = 0;
            string token;
            while((pos = arg.find(delimiter)) != string::npos){
                token = arg.substr(0,pos);
                if(!token.empty())
                    ctx->addReqDevice(token);
                arg.erase(0,pos+delimiter.length());
            }
            if(!arg.empty())
                ctx->addReqDevice(arg);
        }
        // ROOTFS parse
        else if (i == argc - 1 && !regex_match(argv[i], r_rfs)) {
            ctx->setRootFs(string(argv[i]));
        }
    }
    return ctx->validate() && ctx->parseOwner();
}

// Configure only calls requestDevice of Manager class
bool configure_command(Context* ctx){
    Manager manager(ctx);
    if (!manager.requestDevices()){
        manager.releaseDevices();
        errx(ERR_CODE_PARSE_ERROR,"requesting devices failed");
        return false;
    }
    return true;
}