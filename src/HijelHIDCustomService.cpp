#include "HijelHIDCustomService.h"
#include <cstring>

namespace {

class CustomServiceCallbacks : public NimBLECharacteristicCallbacks {
public:
    CustomServiceCallbacks(HijelHID_CustomService* parent) : _parent(parent) {}
    void onWrite(NimBLECharacteristic* pChar, NimBLEConnInfo& connInfo) override {
        (void)connInfo;
        _parent->onCommandWrite(pChar);
    }
private:
    HijelHID_CustomService* _parent;
};

} // namespace

void HijelHID_CustomService::setServiceUUID(const char* svc) {
    _svcUuid = (svc != nullptr) ? svc : "";
}

void HijelHID_CustomService::setCommandUUID(const char* cmd) {
    _cmdUuid = (cmd != nullptr) ? cmd : "";
}

void HijelHID_CustomService::setStatusUUID(const char* sts) {
    _stsUuid = (sts != nullptr) ? sts : "";
}

void HijelHID_CustomService::setCommandCallback(void (*cb)(const uint8_t*, size_t)) {
    _cbCommand = cb;
}

void HijelHID_CustomService::begin(NimBLEServer* server) {
    if (!isEnabled() || _pSts != nullptr || _pCmd != nullptr) return;  // already created

    NimBLEService* svc = server->createService(_svcUuid.c_str());
    if (!_cmdUuid.empty()) {
        _pCmd = svc->createCharacteristic(_cmdUuid.c_str(),
                                          NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    }
    if (!_stsUuid.empty()) {
        _pSts = svc->createCharacteristic(_stsUuid.c_str(),
                                          NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    }
    if (_pCmd != nullptr) {
        _pCb = new CustomServiceCallbacks(this);  // owned forever, freed only by kill()
        _pCmd->setCallbacks(_pCb);
    }
}

void HijelHID_CustomService::onCommandWrite(NimBLECharacteristic* pChar) {
    if (_cbCommand != nullptr) {
        NimBLEAttValue val = pChar->getValue();
        _cbCommand(val.data(), val.size());
    }
}

void HijelHID_CustomService::notifyStatus(const uint8_t* data, size_t len) {
    if (_pSts != nullptr) {
        _pSts->setValue(data, len);
        _pSts->notify(data, len);
    }
}

void HijelHID_CustomService::notifyStatus(const char* str) {
    notifyStatus(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

void HijelHID_CustomService::notifyStatus(uint8_t byte) {
    notifyStatus(&byte, 1);
}
