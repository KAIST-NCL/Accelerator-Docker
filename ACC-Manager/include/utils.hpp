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
#include <limits.h>

#include <pwd.h>
#include <grp.h>

#include "common.hpp"
#include <regex>
#include <vector>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <cctype>

typedef string (*parseFn)(string, string &, string);

bool caseInSensStringCompare(string , string);

string joinRootfsPath(const string &, const string &);

string cgroupMount(string, string &, string);
string cgroupRoot(string, string &, string);

int parseNamespace(int);
bool enterNamespace(int, int*);
bool setCgroup(int, struct stat, const string &);

int makeAncestors(string, mode_t);
mode_t getUmask();

string parseProcFile(string , parseFn, string &, string);
string findCgroupPath(pid_t);

bool isFileExisting(const char*);

string generateDeviceId(Device);
#endif

