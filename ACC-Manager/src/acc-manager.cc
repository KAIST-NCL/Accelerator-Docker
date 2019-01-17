#include "cli.hpp"

#define COMMAND_CNT 4

Command command_list[COMMAND_CNT] = {
    { "configure", &configure_parse, &configure_command },
    { "release", &release_parse, &release_command},
    { "list", &list_parse, &list_command },
    { "help", &help_parse, &help_command}
};

//TODO : Error handling --> using exception
//TODO : Set status file, device file path with given option '--config= , --stat='
int main(int argc, char** argv)
{
    int index, argc_n;
    char** argv_n;

    Context ctx((uid_t)-1, (gid_t)-1);
    ctx.setDeviceFilePath((char*)USR_DEF_DEV);
    ctx.setStatusFilePath((char*)STATUS_CFG);

    char** it;
    for (it = argv; it < argv + argc; it++) {
        for (const auto &cmd : command_list) {
            string arg(it[0]);
            if (arg == cmd.name) {
                index = (int)(it - argv);
                argv_n = it;
                argc_n = argc - index;

                // First parse command line for given command
                if (!cmd.parse_fn(argc_n, argv_n, &ctx)) {
                  // Print out help
                  help_command(nullptr);
                  return ERR_CODE_PARSE_ERROR;
                }
                // Execute the command
                if (!cmd.func(&ctx)) {
                  help_command(nullptr);
                  return ERR_CODE_FUNC_ERROR;
                }
                return EXIT_CODE_SUCCESS;
            }
        }
    }
    help_command(&ctx);
    return 0;
}
