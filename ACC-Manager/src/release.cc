#include "cli.hpp"
#include "manager.hpp"

// Release format
// : acc-manager release --pid=<PID>
bool release_parse(int argc, char** argv, Context* ctx)
{
    for (int i = 0; i < argc; i++) {
        cmatch cm;
        regex r_pid("--pid=([0-9]*)");
        // Parse PID
        if (regex_match(argv[i], cm, r_pid) && cm.size() > 1) {
            ctx->setPid( atoi(cm[1].str().c_str()) );
        }
    }
    if(ctx->getPid() < 0){
        errx(ERR_CODE_PID_NOT_VALID,"Not valid pid");
        return false;
    }
    return true;
}

// Release all devices by calling manager
bool release_command(Context* ctx)
{
    Manager manager(ctx);
    if (!manager.releaseDevices()){
        errx(ERR_CODE_RELEASE_DEVICE_FAILED,"Releasing devices failed");
        return false;
    }
    return true;
}