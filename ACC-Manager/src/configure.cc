#include "acc-manager.hpp"
#include "cli.hpp"
#include "manager.hpp"
#include "mounter.hpp"

bool configure_parse(int argc, char** argv, Context* ctx){
    for (int i = 0; i < argc; i++) {
        cmatch cm;
        regex r_pid("--pid=([0-9]*)");
        regex r_dev("--device=(.*)");
        regex r_rfs("^--");
        if (regex_match(argv[i], cm, r_pid) && cm.size() > 1) {
            ctx->setPid( atoi(cm[1].str().c_str()) );
        } else if (regex_match(argv[i], cm, r_dev) && cm.size() > 1) {
            string arg = cm[1].str();
            string delimiter = ",";
            size_t pos = 0;
            string token;
            while((pos = arg.find(delimiter)) != string::npos){
                token = arg.substr(0,pos);
                if(token.compare("") != 0)
                    ctx->addReqDevice(token);
                arg.erase(0,pos+delimiter.length());
            }
            if(arg.compare("") != 0)
                ctx->addReqDevice(arg);
        } else if (i == argc - 1 && !regex_match(argv[i], r_rfs)) {
            ctx->setRootFs(string(argv[i]));
        }
    }
    return ctx->validate() && ctx->parseOwner();
}

bool configure_command(Context* ctx){
    Manager manager(ctx);
    if (!manager.requestDevices()){
        manager.releaseDevices();
        errx(ERR_CODE_PARSE_ERROR,"requesting devices failed");
        return false;
    }
    return true;
}