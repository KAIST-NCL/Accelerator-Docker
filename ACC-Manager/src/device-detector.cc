#include "device-detector.hpp"

using namespace std;
using namespace google;

// Device Detector
// : Auto-detect devices using PCI-e bus info., etc.
// not implemented yet
DeviceDetector::DeviceDetector() = default;

// Detect and move detected device list to 'to'
bool DeviceDetector::detect(list<Accelerator>* to){
    list<Accelerator> accs;
    //TODO : implement detect method
    *to = accs;
    return true;
}

// Detect and append detected device list to '_acc' and move it to 'to'
bool DeviceDetector::detect(list<Accelerator>* to, list<Accelerator> _acc){
    list<Accelerator> accs;
    detect(&accs);
    _acc.insert(_acc.end(),accs.begin(),accs.end());
    *to = _acc;
    return true;
}