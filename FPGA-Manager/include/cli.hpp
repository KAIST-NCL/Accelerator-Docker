#ifndef _ACC_MANAGER_CLI
#define _ACC_MANAGER_CLI

#include <algorithm>
#include <regex>
#include "acc-manager.hpp"
#include "utils.hpp"

#define EXIT_CODE_SUCCESS 0
#define ERR_CODE_PARSE_ERROR 1
#define ERR_CODE_FUNC_ERROR 2
#define ERR_CODE_COMMAND_ERROR 3

class Context;

//extern const struct argp configure_usage;
bool configure_parse(int, char**, Context *);
bool configure_command(Context *);

//extern const struct argp release_usage;
bool release_parse(int, char**, Context *);
bool release_command(Context *);

//extern const struct argp list_usage;
bool list_parse(int, char**, Context *);
bool list_command(Context *);

class Command {
    public:
        Command(string name, bool (*)(int, char**,Context*), bool (*)(Context*));
        string name;
        bool (*parse_fn)(int argc, char** argv,Context*);
        bool (*func)(Context *ctx);
};

#endif