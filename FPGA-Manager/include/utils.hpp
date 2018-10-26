#ifndef _ACC_MANAGER_UTILS
#define _ACC_MANAGER_UTILS

#include <sys/capability.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>
#include <libgen.h>

#include <pwd.h>
#include <grp.h>

#include "common.hpp"

#include <fstream>
#include <sstream>

#define nitems(x) (sizeof(x) / sizeof(*x))

typedef char *(*parseFn)(char *, char *, const char *);

void str_lower(char *);
bool str_equal(const char *, const char *);
bool str_case_equal(const char *, const char *);
bool str_has_prefix(const char *, const char *);
bool str_has_suffix(const char *, const char *);
bool str_empty(const char *);
bool str_array_match(const char *, const char * const [], size_t);
int  str_to_pid(const char *str, pid_t *pid);
int  str_to_ugid(char *, uid_t *, gid_t *);
int  str_join(char **, const char *, const char *);

string join_rootfs_path(string, string);

static char * cgroupMount(char *line, char *prefix, const char *subsys);
static char * cgroupRoot(char *line, char *prefix, const char *subsys);

int parseNamespace(int pid);
bool enterNamespace(int pid, int* _fd);
bool setCgroup(int pid, struct stat s, char * cg_path);

int makeAncestors(char *path, mode_t perm);
mode_t getUmask(void);

char * parseProcFile(const char *procf, parseFn parse, char *prefix, const char *subsys);
char * findCgroupPath(pid_t pid);

bool isFileExisting(const char*);

#endif