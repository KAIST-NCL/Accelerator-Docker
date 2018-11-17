#include "cli.hpp"

Command::Command(string _name, bool (*_parse_fn)(int,char**,Context*), bool (*_func)(Context*)) : name(_name), parse_fn(_parse_fn), func(_func){}

bool help_parse(int argc, char** argv, Context *ctx){
    return true;
}
bool help_command(Context *ctx){

    printf("\n");
    printf("+------+--------------------+----------+----------------+\n");
    printf("|                 FPGA-Docker  Manager                  |\n");
    printf("+------+--------------------+----------+----------------+\n");
    printf(" FPGA-Docker provides FPGA-Manager which gives \n information about status of accelerators.\n");
    printf(" To execute the manager, please enter the option name.\n");
    printf("+------+--------------------+----------+----------------+\n\n");
    printf(" \t <how to use FPGA-Manager > \n\n");
    printf(" fpga-manager [OPTION1] \n\n");
    printf(" [OPTION1] \n\n");
    printf("    help:   print help instructions\n");
    printf("    list:   print list of accelerators and their status \n\n");
    printf("+------+--------------------+----------+----------------+\n\n");
    printf("\t <how to use FPGA-Docker > \n\n");
    printf(" docker run --runtime fpga-runtime -e ACC_VISIBLE_DEVICES={NAME}\n\n\n");
    return true;
}
