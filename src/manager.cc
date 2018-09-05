#include "manager.hpp"

Manager::Manager(Context* _ctx,Container* _cont) : ctx(_ctx), cont(_cont){
}

bool Manager::requestDevices(){
    list<Device> devs;
    if(!parseAndDetectDevices(&devs)){
        errx(ERR_CODE_PARSE_DETECT_DEVICES_FAILED,"detect and parsing devices failed");
        return false;
    }
    list<string> req_devs = ctx->devices_name;
    
    if(req_devs.size() == 0)
        req_devs.push_back(string("all"));
    for(list<string>::iterator it_st = req_devs.begin() ; it_st != req_devs.end() ; ++it_st){
        bool result_it = false;
        for(list<Device>::iterator it_dev = devs.begin() ; !result_it && it_dev != devs.end() ; ++ it_dev){
            if(it_st->compare(it_dev->getName()) == 0 || it_st->compare(string("all")) == 0){
                if(it_dev->getStatus() == Device::Status::UNAVAILABLE){
                    errx(ERR_CODE_DEVICE_LOCKED,"[%s] device locked by other process",it_st->c_str());
                    return false;
                }
                result_it = true;
                ctx->devices.push_back(*it_dev);
                requestDevice(*it_dev);
            }
        }
        if(!result_it){
            errx(ERR_CODE_DEVICE_NOT_FOUND,"[%s] device not found",it_st->c_str());
            return false;
        }
    }
    return true;
}
bool Manager::requestDevice(Device dev){
    dev.setPid(cont->getPid());
    dev.setStatus(Device::Status::UNAVAILABLE);
    updateStatusFile();
    return true;
}
bool Manager::releaseDevices(){
    list<Device> devs;
    if(!parseAndDetectDevices(&devs)){
        //errx();
        return false;
    }
    return false;
}

bool Manager::updateStatusFile(){

}
bool Manager::parseAndDetectDevices(list<Device> *bin){
    const char* tmp_config_file = "/etc/fpga-docker/device.pbtxt";
    DeviceParser parser((char*)tmp_config_file);
    DeviceDetector detector;

    list<Device> device_list;
    *bin = detector.detect(device_list);
    return true;
}