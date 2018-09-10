#include "cli.hpp"

#define COMMAND_CNT 3

Command command_list[COMMAND_CNT] = {
    { "configure", &configure_parse, &configure_command },
    { "release", &release_parse, &release_command},
    { "list", &list_parse, &list_command }
};

int main(int argc, char** argv)
{
    int index, argc_n;
    char** argv_n;

    Context ctx((uid_t)-1, (gid_t)-1);
    ctx.setDeviceFilePath((char*)USR_DEF_DEV);
    ctx.setStatusFilePath((char*)STATUS_CFG);

    char** it;
    for (it = argv; it < argv + argc; it++) {
        for (int i = 0; i < COMMAND_CNT; i++) {
            string arg(it[0]);
            if (arg.compare(command_list[i].name) == 0) {
                index = it - argv;
                argv_n = it;
                argc_n = argc - index;

                if (!command_list[i].parse_fn(argc_n, argv_n, &ctx)) {
                    errx(ERR_CODE_PARSE_ERROR,"[%s] command parsing error",it[0]);
                    return ERR_CODE_PARSE_ERROR;
                }
                if (!command_list[i].func(&ctx)) {
                    errx(ERR_CODE_FUNC_ERROR,"[%s] command execution error",it[0]);
                    return ERR_CODE_FUNC_ERROR;
                }
                return EXIT_CODE_SUCCESS;
            }
        }
    }
    errx(ERR_CODE_FUNC_ERROR,"command execution error");
    return ERR_CODE_COMMAND_ERROR;
}