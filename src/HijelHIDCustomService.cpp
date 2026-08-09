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

void HijelHID_CustomService::setUUIDs(const char* svc, const char* cmd, const char* sts) {
    if (svc != nullptr && cmd != nullptr && sts != nullptr &&
        svc[0] != '\0' && cmd[0] != '\0' && sts[0] != '\0') {
        _svcUuid = svc;
        _cmdUuid = cmd;
        _stsUuid = sts;
    } else {
        _svcUuid.clear();
        _cmdUuid.clear();
        _stsUuid.clear();
    }
}

void HijelHID_CustomService::setCommandCallback(void (*cb)(const uint8_t*, size_t)) {
    _cbCommand = cb;
}

void HijelHID_CustomService::begin(NimBLEServer* server) {
    if (!isEnabled() || _pSts != nullptr) return;  // already created

    NimBLEService* svc = server->createService(_svcUuid.c_str());
    _pCmd = svc->createCharacteristic(_cmdUuid.c_str(),
                                      NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    _pSts = svc->createCharacteristic(_stsUuid.c_str(),
                                      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

    _pCb = new CustomServiceCallbacks(this);  // owned forever, freed only by kill()
    _pCmd->setCallbacks(_pCb);
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
