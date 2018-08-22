#ifndef _ACC_MANAGER_CLI
#define _ACC_MANAGER_CLI

#include <algorithm>
#include "acc-manager.hpp"
#include "utils.hpp"

class Context;

//extern const struct argp configure_usage;
int configure_parse(int, char**, Context *);
int configure_command(Context *);

//extern const struct argp driver_usage;
int driver_parse(int, char**, Context *);
int driver_command(Context *);

char* getCmdOption(char ** begin, char ** end, const std::string & option);
char** cmdOptionExists(char** begin, char** end, const std::string& option);

class Command {
    public:
        Command(string name, int (*)(int, char**,Context*), int (*)(Context*));
        string name;
        int (*parse_fn)(int argc, char** argv,Context*);
        int (*func)(Context *ctx);
};

class Context {
    public:
        Context();
        Context(uid_t,gid_t);
        /* main */
        uid_t uid;
        gid_t gid;
        string root;
        string ldcache;
        bool load_kmods;
        string init_flags;
        //Command *command;

        /* info */
        bool csv_output;

        /* configure */
        pid_t pid;
        string rootfs;
        string reqs[32];
        size_t nreqs;
        string ldconfig;
        string container_flags;

        /* list */
        bool compat32;
        bool list_bins;
        bool list_libs;
        bool list_ipcs;

        string devices;
};

#endif