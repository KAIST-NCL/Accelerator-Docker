#include "utils.hpp"

bool caseInSensStringCompare(std::string str1, std::string str2){
	return ((str1.size() == str2.size()) && std::equal(str1.begin(), str1.end(), str2.begin(), [](char & c1, char & c2){
        return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
    }));
}

void str_lower(char* str)
{
    for (char* p = str; *p != '\0'; ++p)
        *p = (char)tolower(*p);
}

bool str_equal(const char* s1, const char* s2)
{
    return (!strcmp(s1, s2));
}

bool str_case_equal(const char* s1, const char* s2)
{
    return (!strcasecmp(s1, s2));
}

bool str_has_prefix(const char* str, const char* prefix)
{
    return (!strncmp(str, prefix, strlen(prefix)));
}

bool str_has_suffix(const char* str, const char* suffix)
{
    size_t len, slen;

    len = strlen(str);
    slen = strlen(suffix);
    return ((len >= slen) ? str_equal(str + len - slen, suffix) : false);
}

bool str_empty(const char* str)
{
    return (str != NULL && *str == '\0');
}

bool str_array_match(const char* str, const char* const arr[], size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        if (str_has_prefix(str, arr[i]))
            return (true);
    }
    return (false);
}

int str_join(char** s1, const char* s2, const char* sep)
{
    size_t size = 1;
    char* buf;

    if (*s1 != NULL && **s1 != '\0')
        size += strlen(*s1) + strlen(sep);
    size += strlen(s2);
    if ((buf = (char*)realloc(*s1, size)) == NULL) {
        return (-1);
    }
    if (*s1 == NULL)
        *buf = '\0';
    if (*buf != '\0')
        strcat(buf, sep);
    strcat(buf, s2);
    *s1 = buf;
    return (0);
}

int str_to_pid(const char* str, pid_t* pid)
{
    char* ptr;
    intmax_t n;

    n = strtoimax(str, &ptr, 10);
    if (ptr == str || *ptr != '\0') {
        goto fail;
    }
    if (n == INTMAX_MIN || n == INTMAX_MAX || n != (pid_t)n) {
        goto fail;
    }
    *pid = (pid_t)n;
    return (0);

fail:
    return (-1);
}

int str_to_ugid(char* str, uid_t* uid, gid_t* gid)
{
    char* ptr;
    uintmax_t n;
    struct passwd* passwd = NULL;
    struct group* group = NULL;

    n = strtoumax(str, &ptr, 10);
    if (ptr != str) {
        if (*ptr != '\0' && *ptr != ':') {
            goto fail;
        }
        if (n == UINTMAX_MAX || n != (uid_t)n) {
            goto fail;
        }
        if (*ptr == ':')
            ++ptr;
        *uid = (uid_t)n;
    } else {
        /* Not a numeric UID, check for a username. */
        if ((ptr = strchr(str, ':')) != NULL)
            *ptr++ = '\0';
        if ((passwd = getpwnam(str)) == NULL) {
            goto fail;
        }
        *uid = passwd->pw_uid;
    }

    str = ptr;
    if (str == NULL || *str == '\0') {
        /* No group specified, infer it from the UID */
        if (passwd == NULL && (passwd = getpwuid(*uid)) == NULL) {
            goto fail;
        }
        *gid = passwd->pw_gid;
        return (0);
    }

    n = strtoumax(str, &ptr, 10);
    if (ptr != str) {
        if (*ptr != '\0') {
            goto fail;
        }
        if (n == UINTMAX_MAX || n != (gid_t)n) {
            goto fail;
        }
        *gid = (gid_t)n;
    } else {
        /* Not a numeric GID, check for a groupname. */
        if ((group = getgrnam(str)) == NULL) {
            goto fail;
        }
        *gid = group->gr_gid;
    }
    return (0);

fail:
    return (-1);
}

string join_rootfs_path(string rootfs, string path)
{
    stringstream ss;
    string result = rootfs;
    while(result.c_str()[result.size()-1] == '/'){
        result = result.substr(0,result.size()-1);
    }
    ss << result << path;
    return ss.str();
}

bool setCgroup(int pid, struct stat s, char * cg_path){
    char dev_allow_path[PATH_LEN_MAX];
    strcpy(dev_allow_path, cg_path);
    strcat(dev_allow_path, "/devices.allow");

    FILE *dev_allow;
    dev_allow = fopen(dev_allow_path, "a");
    if(dev_allow == 0)
        return false;
    fprintf(dev_allow, "c %u:%u rw", major(s.st_rdev), minor(s.st_rdev));
    fclose(dev_allow);
    return true;
}

char* findCgroupPath(pid_t pid){
    char path[PATH_LEN_MAX];
    char root_prefix[PATH_LEN_MAX];
    char *mount = NULL;
    char *root = NULL;
    char *cgroup = NULL;

    if (snprintf(path, sizeof(path), "/proc/%d/mountinfo", getppid()) < 0)
        goto fail;
    if ((mount = parseProcFile(path, cgroupMount, root_prefix, "devices")) == NULL)
        goto fail;
    if (snprintf(path, sizeof(path), "/proc/%d/cgroup", pid) < 0)
        goto fail;
    if ((root = parseProcFile(path, cgroupRoot, root_prefix, "devices")) == NULL)
        goto fail;

    asprintf(&cgroup, "%s%s", mount, root);

fail:
    free(mount);
    free(root);
    return (cgroup);
}

char* parseProcFile(const char *procf, parseFn parse, char *prefix, const char *subsys){
    FILE *fs;
    ssize_t n;
    char *buf = NULL;
    size_t len = 0;
    char *ptr = NULL;
    char *path = NULL;

    if ((fs = fopen(procf,"r")) == NULL)
        return (NULL);
    while ((n = getline(&buf, &len, fs)) >= 0){
        ptr = buf;
        ptr[strcspn(ptr, "\n")] = '\0';
        if (n == 0 || *ptr == '\0')
            continue;
        if ((ptr = parse(ptr, prefix, subsys)) != NULL)
            break;
    }
    if (ferror(fs)){
        goto fail;
    }
    if (ptr == NULL || feof(fs)){
        goto fail;
    }
    path = strdup(ptr);

fail:
    free(buf);
    fclose(fs);
    return (path);
}

int parseNamespace(int pid){
    char ns_path[PATH_LEN_MAX];
    int fd;
    if (pid == 0){
        snprintf(ns_path, sizeof(ns_path), "/proc/self/ns/mnt");
    }else{
        snprintf(ns_path, sizeof(ns_path), "/proc/%d/ns/mnt", pid);
    }
    fd = open(ns_path, O_RDONLY | O_CLOEXEC);
    return fd;
}

bool enterNamespace(int pid, int *_fd){
    int fd;
    if (_fd == 0){
        fd = parseNamespace(pid);
    }else{
        fd = *_fd;
    }
    setns(fd, CLONE_NEWNS);
    close(fd);
    return true;
}

int makeAncestors(char *path, mode_t perm){
    struct stat s;
    char *p;

    if (*path == '\0' || *path == '.')
        return (0);

    if (stat(path, &s) == 0){
        if (S_ISDIR(s.st_mode))
            return (0);
        errno = ENOTDIR;
    }
    if (errno != ENOENT)
        return (-1);

    if ((p = strrchr(path, '/')) != NULL){
        *p = '\0';
        if (makeAncestors(path, perm) < 0)
            return (-1);
        *p = '/';
    }
    return (mkdir(path, perm));
}

mode_t getUmask(void){
    mode_t mask;

    mask = umask(0);
    umask(mask);
    return (mask);
}

static char *cgroupMount(char *line, char *prefix, const char *subsys){
    char *root, *mount, *fstype, *substr;

    for (int i = 0; i < 4; ++i)
        root = strsep(&line, " ");
    mount = strsep(&line, " ");
    line = strchr(line, '-');
    for (int i = 0; i < 2; ++i)
        fstype = strsep(&line, " ");
    for (int i = 0; i < 2; ++i)
        substr = strsep(&line, " ");

    if (root == NULL || mount == NULL || fstype == NULL || substr == NULL)
        return (NULL);
    if (*root == '\0' || *mount == '\0' || *fstype == '\0' || *substr == '\0')
        return (NULL);
    if (!str_equal(fstype, "cgroup"))
        return (NULL);
    if (strstr(substr, subsys) == NULL)
        return (NULL);
    if (strlen(root) >= PATH_LEN_MAX || str_has_prefix(root, "/.."))
        return (NULL);
    strcpy(prefix, root);

    return (mount);
}

static char *cgroupRoot(char *line, char *prefix, const char *subsys){
    char *root, *substr;

    for (int i = 0; i < 2; ++i)
        substr = strsep(&line, ":");
    root = strsep(&line, ":");

    if (root == NULL || substr == NULL)
        return (NULL);
    if (*root == '\0' || *substr == '\0')
        return (NULL);
    if (strstr(substr, subsys) == NULL)
        return (NULL);
    if (strlen(root) >= PATH_LEN_MAX || str_has_prefix(root, "/.."))
        return (NULL);
    if (!str_equal(prefix, "/") && str_has_prefix(root, prefix))
        root += strlen(prefix);

    return (root);
}

bool isFileExisting(const char* path){
    struct stat s;
    return (stat (path, &s) == 0); 
}

unsigned hash_str(const char* s)
{
   unsigned h = 37;
   while (*s) {
     h = (h * 54059) ^ (s[0] * 76963);
     s++;
   }
   return h % 86969;
}
//Generate device id by hashing device/library file list --> if device/library file list changes, id changes
string generateDeviceId(Device dev){
    unsigned hash = 0;
    for(string path: dev.getDeviceFiles()){
        hash += hash_str(path.c_str());
    }
    for(string path: dev.getLibraries()){
        hash += hash_str(path.c_str());
    }
    return to_string(hash);
}