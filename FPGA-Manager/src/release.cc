#include "acc-manager.hpp"
#include "cli.hpp"
#include "manager.hpp"

bool release_parse(int argc, char** argv, Context* ctx)
{
    for (int i = 0; i < argc; i++) {
        cmatch cm;
        regex r_pid("--pid=([0-9]*)");
        if (regex_match(argv[i], cm, r_pid) && cm.size() > 1) {
            ctx->setPid( atoi(cm[1].str().c_str()) );
        }
    }
    if(ctx->getPid() < 0){
        errx(ERR_CODE_PID_NOT_VALID,"not valid pid");
        return false;
    }
    return true;
}

bool release_command(Context* ctx)
{
    Manager manager(ctx);
    if (!manager.releaseDevices()){
        errx(ERR_CODE_RELEASE_DEVICE_FAILED,"releasing devices failed");
        return false;
    }
    return true;
}