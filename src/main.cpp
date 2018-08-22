#include "cli.hpp"
#include <iostream>

#define COMMAND_CNT 2

Command command_list[COMMAND_CNT] = {
        {"configure",&configure_parse, &configure_command},
        {"driver",&driver_parse, &driver_command}
};

int main(int argc, char *argv[]){
    Context ctx((uid_t)-1, (gid_t)-1);
    
    int index, argc_n;
    char** argv_n;
    char** command_options;
    for(int i = 0 ; i < COMMAND_CNT ; i++){
        command_options = cmdOptionExists(argv,argv+argc,command_list[i].name);
        if(command_options != 0){
                index = command_options - argv;
                argv_n = command_options;
                argc_n = argc - index;

                if(command_list[i].parse_fn(argc_n,argv_n,&ctx) < 0)
                        return 1;
                cout << "hi1" << endl;
                if(command_list[i].func(&ctx) < 0)
                        return 1;
                cout << "hi2" << endl;
                return 0;
        }
    }
    return 0;
}