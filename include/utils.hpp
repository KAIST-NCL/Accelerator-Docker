#ifndef _ACC_MANAGER_UTILS
#define _ACC_MANAGER_UTILS

#include <sys/capability.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include <pwd.h>
#include <grp.h>

#include "common.hpp"

#define nitems(x) (sizeof(x) / sizeof(*x))

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

#endif