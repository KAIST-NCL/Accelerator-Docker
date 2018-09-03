#include "cli.hpp"

Command::Command(string _name, int (*_parse_fn)(int,char**,Context*), int (*_func)(Context*)) : name(_name), parse_fn(_parse_fn), func(_func){}

Context::Context(){}
Context::Context(uid_t _uid, gid_t _gid) : uid(_uid),gid(_gid){}

char* getCmdOption(char ** begin, char ** end, const std::string & option){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        return *itr;
    }
    return 0;
}

char** cmdOptionExists(char** begin, char** end, const std::string& option){
    char ** itr = std::find(begin, end, option);
    if (itr != end){
        return itr;
    }
    return 0;
}