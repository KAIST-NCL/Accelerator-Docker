#include "manager.hpp"

Manager::Manager(Context* _ctx) : ctx(_ctx){
    if(!parseAndDetectAccelerators()){
        errx(ERR_CODE_PARSE_DETECT_DEVICES_FAILED,"detect and parsing devices failed");
    }
}

bool Manager::requestDevices(){
    list<string> req_devs = ctx->getReqDevices();
    for(list<string>::iterator it_st = req_devs.begin() ; it_st != req_devs.end() ; ++it_st){
        bool result_it = false;
        for(list<Accelerator>::iterator it_acc = acc_list.begin(); !result_it && it_acc != acc_list.end(); it_acc++){
            list<Device> dev_list = it_acc->getDevices();
            for(list<Device>::iterator it_dev = dev_list.begin() ; !result_it && it_dev != dev_list.end() ; ++ it_dev){
                // if given 'ACC_VISIBLE_DEVICES' equal to device name or id or 'all', provide the device
                if(caseInSensStringCompare(*it_st,it_dev->getName()) || caseInSensStringCompare(*it_st,it_dev->getId()) || caseInSensStringCompare(*it_st,string("all"))){
                    if(it_dev->getStatus() == Device::Status::UNAVAILABLE){
                        errx(ERR_CODE_DEVICE_LOCKED,"[%s] device locked by other process",it_st->c_str());
                        return false;
                    }
                    if(it_dev->getStatus() == Device::Status::MISCONFIGURED){
                        errx(ERR_CODE_DEVICE_MISCONFIGURED,"[%s] device is unable to use as it is misconfigured",it_st->c_str());
                        return false;
                    }
                    result_it = true;
                    //err
                    requestDevice(it_dev);
                }
            }
            if(result_it){
                it_acc->setDevices(dev_list);
                updateStatusFile();
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
    Mounter mounter(ctx);
    dev->setPid(ctx->getPid());
    dev->setStatus(Device::Status::UNAVAILABLE);
    if(!mounter.mountDevice(*dev)){
        releaseDevices();
        errx(1,"mounting devices failed");
        return false;
    }
    return true;
}

// Release every devices held by all dead processes
bool Manager::releaseDevices(){
    for(list<Accelerator>::iterator it_acc = acc_list.begin(); it_acc != acc_list.end(); it_acc++){
        list<Device> dev_list = it_acc->getDevices();
        for(list<Device>::iterator it_dev = dev_list.begin() ; it_dev != dev_list.end() ; ++ it_dev){
            //No permission issue for 'kill' because this function is only called by docker running with root permission
            if(it_dev->getStatus() == Device::Status::UNAVAILABLE && kill(it_dev->getPid(),0) < 0 ){
                it_dev->setStatus(Device::Status::AVAILABLE);
                it_dev->setPid(0);
            }
        }
        it_acc->setDevices(dev_list);
    }
    updateStatusFile();
    return true;
}

bool Manager::updateStatusFile(){
    device::device_list *stat_list = new device::device_list();
    for(list<Accelerator>::iterator it_acc = acc_list.begin(); it_acc != acc_list.end(); it_acc++){
        list<Device> dev_list = it_acc->getDevices();
        for(list<Device>::iterator it_dev = dev_list.begin(); it_dev != dev_list.end(); it_dev++){
            device::device *dev = stat_list->add_devices();
            //only need to store name, status, pid, id
            dev->set_name(it_dev->getName());
            dev->set_id(generateDeviceId(*it_dev));
            dev->set_status(it_dev->getStatus() == Device::Status::AVAILABLE ? device::device_Status::device_Status_IDLE : device::device_Status::device_Status_USED);
            dev->set_pid(it_dev->getPid());
        }
    }
    
    fstream output(ctx->getStatusFilePath(), ios::out | ios::trunc | ios::binary);
    if(!stat_list->SerializeToOstream(&output)){
        errx(1,"write file error");
        return false;
    }
    return true;
}
bool Manager::parseAndDetectAccelerators(){
    DeviceParser parser((char*)ctx->getStatusFilePath(), (char*)ctx->getDeviceFilePath());
    DeviceDetector detector;

    list<Accelerator> tmp_list;
    parser.parse(&tmp_list);
    detector.detect(&acc_list,tmp_list);
    return true;
}

list<Accelerator> Manager::getAcceleratorList(){
    return acc_list;
}