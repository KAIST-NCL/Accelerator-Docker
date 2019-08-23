#include "mounter.hpp"
#include <iostream>
#include <fstream>

// Mounter Class
// : Mount files needed for accelerators
// : Context has information of container (rootfs, UID, GID)
Mounter::Mounter(Context* _cont) : cont(_cont) {}

// Mount files for all devices contained in given list
bool Mounter::mountDevices(list<Device> devices){
    bool result = true;
    for (auto const & it : devices){
        result = result && mountDevice(it);
    }
    return result;
}

// Mount files for a specific device
bool Mounter::mountDevice(Device device){
    bool result = true;
    list<string> dev_files = device.getDeviceFiles();
    list<string> lib_files = device.getLibraries();
    list<array<string,2>> files = device.getFiles();
    list<array<string,2>> envs = device.getEnvs();

    int fd_mnt_ori;
    fd_mnt_ori = parseNamespace(0); // Original namespace of acc-manager process
    enterNamespace(cont->getPid(), nullptr); // Enter container's namespace
    result = result &&
            mountDeviceFiles(dev_files) &&
            mountLibraries(lib_files) &&
            mountFiles(files) &&
            setEnvs(envs);
    enterNamespace(0, &fd_mnt_ori); // Escape from container's namespace
    return result;
}

// Mount all device files(/dev/<FILE>) on rootfs
bool Mounter::mountDeviceFiles(list<string> dev_files){
    string cg_path;
    bool result = true;

    // cgroup path
    cg_path = findCgroupPath(cont->getPid());
    for (auto const & it : dev_files){
        result = result && mountDeviceFile(it,cg_path);
    }
    return result;
}

// Mount a device file on rootfs
bool Mounter::mountDeviceFile(string dev, string cg_path){
    char src[PATH_LEN_MAX];
    char dst[PATH_LEN_MAX];
    struct stat s = {};

    strcpy(src, dev.c_str());
    stat(src, &s);

    strcpy(dst, cont->getRootFs().c_str());
    strcat(dst, src);

    createDev(dst, s); // Create device file on container
    setCgroup(cont->getPid(), s,cg_path); // Allow the namespace can use accelerator file
    return true;
}

// Create device file
bool Mounter::createDev(string dst, struct stat s){
    mode_t mode, perm;
    char *p;

    mode = s.st_mode;

    p = strdup(dst.c_str());
    perm = (0777 & ~getUmask()) | S_IWUSR | S_IXUSR;
    makeAncestors(dirname(p), perm);
    perm = 0644 & ~getUmask() & mode;
    mknod(dst.c_str(), perm | S_IFCHR, s.st_rdev);
    return true;
}

// Mount all library files given in the list
bool Mounter::mountLibraries(list<string> lib_files){
    bool result = true;
    for (auto const & it : lib_files){
        result = result && mountLibrary(it);
    }
    return result;
}

// Mount a specific library file
bool Mounter::mountLibrary(string lib){
    string base = lib.substr(lib.find_last_of("/\\") + 1);
    string dst = "/usr/lib/"+base;
    return mountFile(lib,dst);
}

// Mount all files given in the list
bool Mounter::mountFiles(list<array<string,2>> files){
    bool result = true;
    for (auto const & it : files){
        result = result && mountFile(it.at(0),it.at(1));
    }
    return result;
}

// Mount a file from src to dst_rel
bool Mounter::mountFile(string src,string dst_rel){
    string dst = joinRootfsPath(cont->getRootFs(),dst_rel);   // dst : {ROOTFS}/{DST_REL}
    const char* src_c = src.c_str();
    const char* dst_c = dst.c_str();
    char dir_tmp[PATH_LEN_MAX];
    struct stat mode = {};
    mode_t perm;
    
    if(src.empty() || dst.empty())
        return false;
    if(!src.empty() && src_c[0] != '/' ){
        //TODO: handle relative path
        errx(ERR_CODE_ONLY_ABS_PATH_ALLOWED,"library [%s] should be in absolute path",src_c);
        return false;
    }
    lstat(src_c, &mode);
    strcpy(dir_tmp,dst_c);
    perm = (0777 & ~getUmask()) | S_IWUSR | S_IXUSR;
    makeAncestors(dirname(dir_tmp), perm);

    if(S_ISLNK(mode.st_mode)){ 
        // If it is a link, make a link as same way from host and mount target file (recursively)
        char lnk_buf[1024];
        ssize_t len;
        if( (len = readlink(src_c,lnk_buf,sizeof(lnk_buf)-1)) < 0){
            return false;
        }
        lnk_buf[len] = '\0';
        string lnk_src;
        string lnk_dst;
        // Linked in absolute path
        if(lnk_buf[0] == '/'){
            lnk_dst = joinRootfsPath(cont->getRootFs(),string(lnk_buf));
            lnk_src = string(lnk_buf);
        }
        // Linked in relative path
        else{
            //Tokenize dst_c and append lnk_buf
            const char delimeter[2] = "/";
            char* token;
            char* tmp_tok;

            string tmp_s = dst_rel;
            char* dst_tmp = (char*)tmp_s.c_str();
            lnk_dst = string("");
            token = strtok(dst_tmp,delimeter);
            while(token != nullptr){
                tmp_tok = strtok(nullptr,delimeter);
                if(tmp_tok != nullptr)
                    lnk_dst += string("/") + string(token);
                token = tmp_tok;
            }
            lnk_dst += string("/") + string(lnk_buf);

            tmp_s = src;
            char* src_tmp = (char*)tmp_s.c_str();
            lnk_src = string("");
            token = strtok(src_tmp,delimeter);
            while(token != nullptr){
                tmp_tok = strtok(nullptr,delimeter);
                if(tmp_tok != nullptr)
                    lnk_src += string("/") + string(token);
                token = tmp_tok;
            }
            lnk_src = lnk_src + string("/") + string(lnk_buf);
        }
        symlink(lnk_buf,dst_c); // Make link in container
        return mountFile(lnk_src,lnk_dst);
    }else if(S_ISREG(mode.st_mode) || S_ISDIR(mode.st_mode)){
        if(S_ISREG(mode.st_mode)){
            std::ofstream dest(dst_c, std::ios::binary);
            dest.close();
        }
        else{
            mkdir(dst_c,perm);
        }
        mount(src_c,dst_c,NULL,MS_BIND,NULL);
        mount(NULL,dst_c,NULL,MS_BIND | MS_REMOUNT | MS_RDONLY | MS_NODEV | MS_NOSUID | MS_REC,NULL);
        return true;
    }else{
        return false;
    }
    return false;
}

// Set env. var
// Temporal naive way (storing /.tmp_env file from acc-manager and parse it at acc-runtime)
bool Mounter::setEnvs(list<array<string,2>> envs){
    string dst = joinRootfsPath(cont->getRootFs(),"/.tmp_env");

    ofstream outFile;
    outFile.open(dst, fstream::out | fstream::app);
    if(outFile.is_open()){
        for(auto const & it : envs){
            outFile << it.at(0) << "=" << it.at(1) << endl;
        }
        outFile.close();
    }
    return true;
}

