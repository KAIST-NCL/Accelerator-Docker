//#include <alloca.h>
//#include <err.h>
#include <unistd.h>
#include <fcntl.h>

//#include <sys/sysmacros.h>
//#include <sys/mount.h>
#include <sys/stat.h>
//#include <sys/types.h>

//#include <sched.h>
#include <libgen.h>

//#include <string.h>

#include <iostream>
#include <fstream>

#include "acc-manager.hpp"
#include "cli.hpp"

#define PATH_MAX 1024

static int make_ancestors(char *path, mode_t perm) {
        struct stat s;
        char *p;

        if (*path == '\0' || *path == '.')
                return (0);

        if (stat(path, &s) == 0) {
                if (S_ISDIR(s.st_mode))
                        return (0);
                errno = ENOTDIR;
        }
        if (errno != ENOENT)
                return (-1);

        if ((p = strrchr(path, '/')) != NULL) {
                *p = '\0';
                if (make_ancestors(path, perm) < 0)
                        return (-1);
                *p = '/';
        }
        return (mkdir(path, perm));
}

static mode_t get_umask(void) {
        mode_t mask;

        mask = umask(0);
        umask(mask);
        return (mask);
}

static char * cgroup_mount(char *line, char *prefix, const char *subsys) {
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
        if (strlen(root) >= PATH_MAX || str_has_prefix(root, "/.."))
                return (NULL);
        strcpy(prefix, root);

        return (mount);
}

static char * cgroup_root(char *line, char *prefix, const char *subsys) {
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
        if (strlen(root) >= PATH_MAX || str_has_prefix(root, "/.."))
                return (NULL);
        if (!str_equal(prefix, "/") && str_has_prefix(root, prefix))
                root += strlen(prefix);

        return (root);
}

typedef char *(*parse_fn)(char *, char *, const char *);
static char * parse_proc_file(const char *procf, parse_fn parse, char *prefix, const char *subsys) {
        FILE *fs;
        ssize_t n;
        char *buf = NULL;
        size_t len = 0;
        char *ptr = NULL;
        char *path = NULL;

        if ((fs = fopen(procf, "r")) == NULL)
                return (NULL);
        while ((n = getline(&buf, &len, fs)) >= 0) {
                ptr = buf;
                ptr[strcspn(ptr, "\n")] = '\0';
                if (n == 0 || *ptr == '\0')
                        continue;
                if ((ptr = parse(ptr, prefix, subsys)) != NULL)
                        break;
        }
        if (ferror(fs)) {
                goto fail;
        }
        if (ptr == NULL || feof(fs)) {
                goto fail;
        }
        path = strdup(ptr);

 fail:
        free(buf);
        fclose(fs);
        return (path);
}

static char * find_cgroup_path(pid_t pid){
        char path[PATH_MAX];
        char root_prefix[PATH_MAX];
        char *mount = NULL;
        char *root = NULL;
        char *cgroup = NULL;

        if (snprintf(path, sizeof(path), "/proc/%d/mountinfo", pid) < 0)
                goto fail;
        if ((mount = parse_proc_file(path, cgroup_mount, root_prefix, "devices")) == NULL)
                goto fail;
        if (snprintf(path, sizeof(path), "/proc/%d/cgroup", pid) < 0)
                goto fail;
        if ((root = parse_proc_file(path, cgroup_root, root_prefix, "devices")) == NULL)
                goto fail;

        asprintf(&cgroup, "%s%s", mount, root);

 fail:
        free(mount);
        free(root);
        return (cgroup);
}

int configure_parse(int argc, char** argv, Context *ctx){
        char* tmp;
        tmp = getCmdOption(argv,argv+argc,"--pid");
        if(tmp == 0){
                return -1;
        }
        ctx->pid = atoi(tmp);
        cout << ctx->pid << endl;
        return 1;
}

int configure_command(Context *ctx){
        ////////////////////////////ACC-Manager Start
        int rv = EXIT_SUCCESS;
        char src[PATH_MAX];
        char dst[PATH_MAX];

        struct stat s_root, s;
        mode_t mode;

        strcpy(src, "/dev/acla10_ref0");
        stat(ctx->rootfs.c_str(),&s_root);
        if(ctx->rootfs.compare("") == 0 || !S_ISDIR(s_root.st_mode))
                return -1;
        strcpy(dst, ctx->rootfs.c_str());
        strcat(dst,src);
        //ns
        char mnt_ns_path[PATH_MAX];
        //store_current_ns
        snprintf(mnt_ns_path, sizeof(mnt_ns_path), "/proc/self/ns/mnt");
        int fd_mnt_ori;
        fd_mnt_ori = open(mnt_ns_path,O_RDONLY|O_CLOEXEC);
        //change_ns
        snprintf(mnt_ns_path, sizeof(mnt_ns_path), "/proc/%d/ns/mnt", ctx->pid);
        int fd_mnt;
        fd_mnt = open(mnt_ns_path,O_RDONLY);
        setns(fd_mnt, CLONE_NEWNS);
        close(fd_mnt);

        //stat & mode
        stat(src,&s);
        mode = s.st_mode;

        //file create
        char *p;
        mode_t perm;
        int fd;
        int flags = O_NOFOLLOW|O_CREAT;

        p = strdup(dst);
        perm = (0777 & ~get_umask()) | S_IWUSR | S_IXUSR;
        make_ancestors(dirname(p), perm);
        //perm = 0777 & ~get_umask() & mode;
        //fd = open(dst, flags, perm);
        //close(fd);
        mknod(dst, (0644 & ~get_umask()) | S_IFCHR, major(s.st_rdev)<<8 + minor(s.st_rdev));
        //chmod(dst,0644);

        //cgroup
        char *cg_path;
        char dev_allow_path[PATH_MAX];
        cg_path = find_cgroup_path(ctx->pid);
        strcpy(dev_allow_path,cg_path);
        strcat(dev_allow_path,"/devices.allow");
        FILE *dev_allow;
        dev_allow = fopen(dev_allow_path,"a");
        fprintf(dev_allow,"c %u:%u rw",major(s.st_rdev),minor(s.st_rdev));
        fclose(dev_allow);

        //ns
        setns(fd_mnt_ori,CLONE_NEWNS);
        close(fd_mnt_ori);

        return (rv);
        ////////////////////////////ACC-Manager End
}