#include "manager.hpp"

// Manager Class
// : Handles require/release of devices by container
// : Context argument has container info (rootfs, required devices)
Manager::Manager(Context* _ctx) : ctx(_ctx){
    // Parse accelerator list at instantiation
    if(!parseAndDetectAccelerators()){
        errx(ERR_CODE_PARSE_DETECT_DEVICES_FAILED,"detect and parsing devices failed");
    }
    updateStatusFile();
}

// Provide pass through of all requested devices to container
bool Manager::requestDevices(){
    list<string> req_devs = ctx->getReqDevices();
    for(auto const & it_st : req_devs){
        bool result_it = false;
        for(auto & it_acc : acc_list){
            list<Device> dev_list = it_acc.getDevices();
            for(auto & it_dev : dev_list){
                // if given 'ACC_VISIBLE_DEVICES' equal to device name or id or 'all', provide the device
                if(caseInSensStringCompare(it_st,it_dev.getName()) || caseInSensStringCompare(it_st,it_dev.getId()) || caseInSensStringCompare(it_st,string("all"))){
                    if(it_dev.getStatus() == Device::Status::UNAVAILABLE && it_dev.getPid() != ctx->getPid()){
                        errx(ERR_CODE_DEVICE_LOCKED,"[%s] device locked by other process",it_st.c_str());
                        return false;
                    }
                    if(it_dev.getStatus() == Device::Status::MISCONFIGURED){
                        errx(ERR_CODE_DEVICE_MISCONFIGURED,"[%s] device is unable to use as it is misconfigured",it_st.c_str());
                        return false;
                    }
                    result_it = true;
                    requestDevice(it_dev);
                }
            }
            if(result_it){
                it_acc.setDevices(dev_list);
                updateStatusFile();
            }
        }
        if(!result_it){
            errx(ERR_CODE_DEVICE_NOT_FOUND,"[%s] device not found",it_st.c_str());
            return false;
        }
    }
    
    return true;
}

// Provide pass through of a specific device to container
bool Manager::requestDevice(Device & dev){
    Mounter mounter(ctx);
    dev.setPid(ctx->getPid());
    dev.setStatus(Device::Status::UNAVAILABLE);
    if(!mounter.mountDevice(dev)){
        releaseDevices();
        errx(1,"mounting devices failed");
        return false;
    }
    return true;
}

// Release every devices held by all dead processes, not only by 'this' process
bool Manager::releaseDevices(){
    for(auto & it_acc : acc_list){
        list<Device> dev_list = it_acc.getDevices();
        for(auto & it_dev : dev_list){
            //No permission issue for 'kill' because this function is only called by docker running with root permission
            if(it_dev.getStatus() == Device::Status::UNAVAILABLE && kill(it_dev.getPid(),0) < 0 ){
                it_dev.setStatus(Device::Status::AVAILABLE);
                it_dev.setPid(0);
            }
        }
        it_acc.setDevices(dev_list);
    }
    updateStatusFile();
    return true;
}

// Update stat.pb file
bool Manager::updateStatusFile(){
    auto *stat_list = new device::device_list();
    for(auto it_acc : acc_list){
        list<Device> dev_list = it_acc.getDevices();
        for(auto it_dev : dev_list){
            device::device *dev = stat_list->add_devices();
            //only need to store name, status, pid, id
            dev->set_name(it_dev.getName());
            dev->set_id(it_dev.getId());
            dev->set_status(it_dev.getStatus() == Device::Status::MISCONFIGURED ? device::device_Status_MISS : it_dev.getStatus() == Device::Status::AVAILABLE ? device::device_Status::device_Status_IDLE : device::device_Status::device_Status_USED);
            dev->set_pid(it_dev.getPid());
        }
    }
    
    fstream output(ctx->getStatusFilePath(), ios::out | ios::trunc | ios::binary);
    if(!stat_list->SerializeToOstream(&output)){
        errx(1,"Writing status file failed");
        return false;
    }
    chmod(ctx->getStatusFilePath(), 0777);
    return true;
}

// Parse device list by calling detector and parser
bool Manager::parseAndDetectAccelerators(){
    DeviceParser parser(ctx->getStatusFilePath(), ctx->getDeviceFilePath());
    DeviceDetector detector;

    list<Accelerator> tmp_list;
    parser.parse(&tmp_list);
    detector.detect(&acc_list,tmp_list);
    return true;
}

list<Accelerator> Manager::getAcceleratorList(){
    return acc_list;
}