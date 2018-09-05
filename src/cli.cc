#include "cli.hpp"

Command::Command(string _name, bool (*_parse_fn)(int,char**,Context*), bool (*_func)(Context*)) : name(_name), parse_fn(_parse_fn), func(_func){}