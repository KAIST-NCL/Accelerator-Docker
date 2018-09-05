#include "mounter.hpp"
#include <iostream>
#include <fstream>

Mounter::Mounter(Container _cont) : cont(_cont) {}
bool Mounter::mountDevices(list<Device> devices){
    bool result = true;
    for (list<Device>::iterator it = devices.begin(); result && it != devices.end(); it++){
        result = result && mountDevice(*it);
    }
    return result;
}

bool Mounter::mountDevice(Device device){
    bool result = true;
    list<string> dev_files = device.getDeviceFilePaths();
    list<LibraryNode> lib_files = device.getLibraries();
    result = result && mountDeviceFiles(dev_files) && mountLibraries(lib_files); 
    return result;
}

bool Mounter::mountDeviceFiles(list<string> dev_files){
    char *cg_path;
    bool result = true;
    
    cg_path = findCgroupPath(cont.getPid());
    for (list<string>::iterator it = dev_files.begin(); result && it != dev_files.end(); it++){
        result = result && mountDeviceFile(*it,cg_path);
    }
    return result;
}
bool Mounter::mountDeviceFile(string dev, char* cg_path){
    int rv = EXIT_SUCCESS;
    char src[PATH_LEN_MAX];
    char dst[PATH_LEN_MAX];
    struct stat s;

    strcpy(src, dev.c_str());
    stat(src, &s);
    //src 있는 파일인지 검사 --> detector에서!

    strcpy(dst, cont.getRootFs().c_str());
    strcat(dst, src);

    int fd_mnt_ori;
    fd_mnt_ori = parseNamespace(0);
    enterNamespace(cont.getPid(), NULL);
    createDev(src, dst, s);
    setCgroup(cont.getPid(), s,cg_path);
    enterNamespace(0, &fd_mnt_ori);
    return true;
}

bool Mounter::mountLibraries(list<LibraryNode> lib_files){
    bool result = true;
    for (list<LibraryNode>::iterator it = lib_files.begin(); result && it != lib_files.end(); it++){
        result = result && mountLibrary(*it);
    }
    return result;
}

bool Mounter::mountLibrary(LibraryNode lib){
    string src = lib.getSrc();
    string dst_rel = lib.getDst();
    string dst = join_rootfs_path(cont.getRootFs(),dst_rel);
    const char* src_c = src.c_str();
    const char* dst_c = dst.c_str();
    char src_tmp[PATH_LEN_MAX],dir_tmp[PATH_LEN_MAX];
    struct stat mode;
    mode_t perm;
    
    /*
        Binary 경로 + $PATH
        Library 경로 + $INTELFPGAOCLSDKROOT
    */
    if(src.empty() || dst.empty())
        return false;
    if(!src.empty() && src_c[0] != '/' ){
        // TODO : realpath, /usr/lib, ...
        errx(ERR_CODE_ONLY_ABS_PATH_ALLOWED,"library [%s] should be in absolute path",src_c);
        return false;
        if(realpath(src_c,src_tmp) == NULL)
            return false;
        src = string(src_tmp);
    }
    lstat(src_c, &mode);
    strcpy(dir_tmp,dst_c);
    perm = (0777 & ~getUmask()) | S_IWUSR | S_IXUSR;
    makeAncestors(dirname(dir_tmp), perm);

    if(S_ISDIR(mode.st_mode) || S_ISLNK(mode.st_mode)){
        return false;
    }else if(S_ISREG(mode.st_mode)){
        std::ifstream srce(src_c, std::ios::binary);
        std::ofstream dest(dst_c, std::ios::binary);
        dest << srce.rdbuf();
        chmod(dst_c,mode.st_mode);
        return true;
    }else{
        return false;
    }
    return false;
}

bool Mounter::createDev(char *src, char *dst, struct stat s){
    mode_t mode, perm;
    char *p;
    int fd;

    mode = s.st_mode;

    p = strdup(dst);
    perm = (0777 & ~getUmask()) | S_IWUSR | S_IXUSR;
    makeAncestors(dirname(p), perm);
    perm = 0644 & ~getUmask() & mode;
    mknod(dst, perm | S_IFCHR, major(s.st_rdev) << 8 + minor(s.st_rdev));
    return true;
}