#include "manager.hpp"

Manager::Manager(Context* _ctx) : ctx(_ctx){
    if(!parseAndDetectDevices()){
        errx(ERR_CODE_PARSE_DETECT_DEVICES_FAILED,"detect and parsing devices failed");
    }
}

bool Manager::requestDevices(){
    Mounter mounter(ctx);
    list<string> req_devs = ctx->getReqDevices();
    
    for(list<string>::iterator it_st = req_devs.begin() ; it_st != req_devs.end() ; ++it_st){
        bool result_it = false;
        for(list<Device>::iterator it_dev = dev_list.begin() ; !result_it && it_dev != dev_list.end() ; ++ it_dev){
            if(it_st->compare(it_dev->getName()) == 0 || it_st->compare(string("all")) == 0){
                if(it_dev->getStatus() == Device::Status::UNAVAILABLE){
                    errx(ERR_CODE_DEVICE_LOCKED,"[%s] device locked by other process",it_st->c_str());
                    return false;
                }
                result_it = true;
                //err
                requestDevice(it_dev);
                if(!mounter.mountDevice(*it_dev)){
                    releaseDevices();
                    errx(1,"mounting devices failed");
                    return false;
                }
            }
        }
        if(!result_it){
            errx(ERR_CODE_DEVICE_NOT_FOUND,"[%s] device not found",it_st->c_str());
            return false;
        }
    }
    
    return true;
}
bool Manager::requestDevice(list<Device>::iterator dev){
    dev->setPid(ctx->getPid());
    dev->setStatus(Device::Status::UNAVAILABLE);
    updateStatusFile();
    return true;
}

// Release every devices held by un-running process
bool Manager::releaseDevices(){
    for(list<Device>::iterator it_dev = dev_list.begin() ; it_dev != dev_list.end() ; ++ it_dev){
        if(it_dev->getStatus() == Device::Status::UNAVAILABLE && kill(it_dev->getPid(),0) < 0 ){
            it_dev->setStatus(Device::Status::AVAILABLE);
            it_dev->setPid(0);
            //delete device files, lib files
        }
    }
    updateStatusFile();
    return true;
}

bool Manager::updateStatusFile(){
    device::device_list *stat_list = new device::device_list();
    for (list<Device>::iterator it = dev_list.begin(); it != dev_list.end(); it++){
        device::device *dev = stat_list->add_devices();
        //only need to store name, status, pid
        dev->set_name(it->getName());
        dev->set_status(it->getStatus() == Device::Status::AVAILABLE ? device::device_Status::device_Status_IDLE : device::device_Status::device_Status_USED);
        dev->set_pid(it->getPid());
    }
    
    fstream output(ctx->getStatusFilePath(), ios::out | ios::trunc | ios::binary);
    if(!stat_list->SerializeToOstream(&output)){
        errx(1,"write file error");
        return false;
    }
    return true;
}
bool Manager::parseAndDetectDevices(){
    DeviceParser parser((char*)ctx->getStatusFilePath(), (char*)ctx->getDeviceFilePath());
    DeviceDetector detector;

    list<Device> tmp_list;
    parser.parse(&tmp_list);
    detector.detect(&dev_list,tmp_list);
    return true;
}

list<Device> Manager::getDeviceList(){
    return dev_list;
}