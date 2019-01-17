#include "utils.hpp"

bool caseInSensStringCompare(string str1, string str2){
	return ((str1.size() == str2.size()) && equal(str1.begin(), str1.end(), str2.begin(), [](char & c1, char & c2){
        return (c1 == c2 || toupper(c1) == toupper(c2));
    }));
}

string joinRootfsPath(const string & rootfs, const string & path){
    stringstream ss;
    string result = rootfs;
    while(result.c_str()[result.size()-1] == '/'){
        result = result.substr(0,result.size()-1);
    }
    ss << result << path;
    return ss.str();
}

bool setCgroup(int pid, struct stat s, const string & cg_path){
    char buffer[PATH_LEN_MAX];
    snprintf(buffer, sizeof(buffer), "c %u:%u rw", major(s.st_rdev), minor(s.st_rdev));

    ofstream outFile;
    outFile.open(cg_path+"/devices.allow",ios::app);
    if(outFile.is_open()){
        outFile << buffer;
    } else
        return false;
    return true;
}

string findCgroupPath(pid_t pid){
    string root_prefix;
    string mount, root;
    string path;

    path = "/proc" + to_string(getppid()) + "/mountinfo";
    mount = parseProcFile(path, cgroupMount, root_prefix, "devices");
    path = "/proc" + to_string(pid) + "/cgroup";
    root = parseProcFile(path, cgroupRoot, root_prefix, "devices");

    return mount+root;
}

string parseProcFile(string procf, parseFn parse, string & prefix, string subsys){
    ifstream inFile(procf);
    string path;
    string line;
    if(inFile.is_open()){
        while(getline(inFile,line)){
            replace(line.begin(),line.end(),'\n','\0');
            if(line.empty())
                continue;
            if(!(path = parse(line,prefix,subsys)).empty())
                break;
        }
    }

    return path;
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
    if (_fd == nullptr){
        fd = parseNamespace(pid);
    }else{
        fd = *_fd;
    }
    setns(fd, CLONE_NEWNS);
    close(fd);
    return true;
}

int makeAncestors(string path, mode_t perm){
    struct stat s = {};

    if(path.empty() || path[0] == '.')
        return (0);

    if (stat(path.c_str(), &s) == 0){
        if (S_ISDIR(s.st_mode))
            return (0);
        errno = ENOTDIR;
    }
    if (errno != ENOENT)
        return (-1);

    size_t pos = path.find_last_of('/');
    if(pos != string::npos){
        string parent = path.substr(0,pos);
        if(makeAncestors(parent,perm) < 0)
            return -1;
    }

    return mkdir(path.c_str(), perm);
}

mode_t getUmask(){
    mode_t mask;

    mask = umask(0);
    umask(mask);
    return (mask);
}

string cgroupMount(string line, string & prefix, string subsys){
    string root, mount, fstype, substr;

    size_t pos_dash = line.find('-');
    if(pos_dash == string::npos)
        return "";
    string first = line.substr(0,pos_dash-1);
    string second = line.substr(pos_dash);
    cmatch cm;

    regex r_first("[^ ]+ [^ ]+ [^ ]+ ([^ ]+) ([^ ]+) .*");
    if (regex_match(first.c_str(), cm, r_first) && cm.size() > 2) {
        root = cm[1];
        mount = cm[2];
    }else{
        return "";
    }

    regex r_second("[^ ]* ([^ ]+) [^ ]+ (.*)");
    if (regex_match(second.c_str(), cm, r_second) && cm.size() > 2) {
        fstype = cm[1];
        substr = cm[2];
    }else{
        return "";
    }

    if(root.empty() || mount.empty() || fstype.empty() || substr.empty())
        return "";
    if(fstype != "cgroup")
        return "";
    if(substr.find(subsys) == string::npos)
        return "";
    if(root.size() >= PATH_LEN_MAX || root.find("/..") == 0)
        return "";

    prefix = root;
    return mount;
}

string cgroupRoot(string line, string & prefix, string subsys){
    string root, substr;
    size_t pos;

    cmatch cm;
    regex reg("[^:]+:([^:]+):([^:]+)");
    if (regex_match(line.c_str(), cm, reg) && cm.size() > 2) {
        substr = cm[1];
        root = cm[2];
    }else{
        return "";
    }

    if(substr.empty() || root.empty())
        return "";
    if(substr.find(subsys) == string::npos)
        return "";
    if(root.size() >= PATH_LEN_MAX || root.find("/..") == 0)
        return "";
    if(prefix != "/" && (pos = root.find(prefix)) == 0){
        root = root.substr(pos);
    }
    return root;
}

bool isFileExisting(const char* path){
    struct stat s = {};
    return (stat (path, &s) == 0); 
}

unsigned hash_str(string s){
   unsigned h = 37;
   for(const char & c : s) {
     h = (h * 54059) ^ (c * 76963);
   }
   return h % 86969;
}

//Generate device id by hashing device/library file list --> if device/library file list changes, id changes
string generateDeviceId(Device dev){
    unsigned hash = 0;
    for(const string & path: dev.getDeviceFiles()){
        hash += hash_str(path);
    }
    for(const string & path: dev.getLibraries()){
        hash += hash_str(path);
    }
    return to_string(hash);
}