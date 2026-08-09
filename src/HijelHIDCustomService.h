#pragma once
/**
 * HijelHIDCustomService.h
 *
 * Optional custom GATT service for HijelHID_BLEKeyboard.
 *
 * By default no custom service is created — the library behaves exactly as
 * before (HID + Battery services only). Call setUUIDs() with your own
 * 128-bit UUIDs before begin() to add a service with two characteristics:
 *
 *   - Command characteristic (write): phone app sends commands to the device.
 *   - Status characteristic  (read/notify): device pushes results to the app.
 *
 * Typical flow: app connects → subscribes to status → writes a command →
 * device responds via notifyStatus().
 *
 * Copyright (c) 2026 Hijel. All rights reserved.
 * Licensed under the Apache License, Version 2.0.
 */

#include <string>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEService.h>
#include <NimBLECharacteristic.h>

class HijelHID_CustomService {
public:
    HijelHID_CustomService() = default;

    /**
     * Enable the custom service with your own service UUID. Call before begin().
     * Passing nullptr/empty disables the service (back to default behaviour).
     */
    void setServiceUUID(const char* svcUuid);

    /** Set the command characteristic UUID (write, phone → device). Optional. */
    void setCommandUUID(const char* cmdUuid);

    /** Set the status characteristic UUID (read/notify, device → phone). Optional. */
    void setStatusUUID(const char* stsUuid);

    /** True when setUUIDs() was called with three valid UUIDs. */
    bool isEnabled() const { return !_svcUuid.empty(); }

    /** Service UUID string, as passed to setUUIDs(). */
    const std::string& serviceUUID() const { return _svcUuid; }

    /**
     * Optional callback fired when the host writes to the command characteristic.
     * `data` is valid only for the duration of the call.
     */
    void setCommandCallback(void (*cb)(const uint8_t* data, size_t len));

    /** Push a status payload to the host via the status characteristic (notify). */
    void notifyStatus(const uint8_t* data, size_t len);
    /** Push a null-terminated status string. */
    void notifyStatus(const char* str);
    /** Push a single status byte. */
    void notifyStatus(uint8_t byte);

    // ── Internal — called by HijelHID_BLEKeyboard::begin() ───────────────
    void begin(NimBLEServer* server);
    void onCommandWrite(NimBLECharacteristic* pChar);

private:
    std::string _svcUuid;
    std::string _cmdUuid;
    std::string _stsUuid;
    NimBLECharacteristic*      _pCmd = nullptr;
    NimBLECharacteristic*      _pSts = nullptr;
    NimBLECharacteristicCallbacks* _pCb = nullptr;
    void (*_cbCommand)(const uint8_t*, size_t) = nullptr;
};
