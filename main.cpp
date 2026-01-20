// Copyright (c) 2025-2026 Half_nothing MIT License

#include "fsuipc_client.h"
#include "fsuipc_export.h"
#include <string>
#include <sstream>
#include <iostream>

FSUIPC::FSUIPCClient client;
FSUIPC::SimConnectionStatus status = FSUIPC::SimConnectionStatus::NO_CONNECTION;
FSUIPC::ApiVersion apiVersion = FSUIPC::ApiVersion::API_UNKNOWN;

FSUIPC::COM1ActiveVer1 com1ActiveVer1;
FSUIPC::COM1StandbyVer1 com1StandbyVer1;
FSUIPC::COM2ActiveVer1 com2ActiveVer1;
FSUIPC::COM2StandbyVer1 com2StandbyVer1;

FSUIPC::COM1ActiveVer2 com1ActiveVer2;
FSUIPC::COM1StandbyVer2 com1StandbyVer2;
FSUIPC::COM2ActiveVer2 com2ActiveVer2;
FSUIPC::COM2StandbyVer2 com2StandbyVer2;

FSUIPC::RadioSwitch radioSwitch;

FSUIPC::WriteDataWORD com1Ver1(com1ActiveVer1.offset, 0);
FSUIPC::WriteDataDWORD com1Ver2(com1ActiveVer2.offset, 0);
FSUIPC::WriteDataWORD com2Ver1(com2ActiveVer1.offset, 0);
FSUIPC::WriteDataDWORD com2Ver2(com2ActiveVer2.offset, 0);

uint32_t com1ActiveLast = 0;
uint32_t com1StandbyLast = 0;
uint32_t com2ActiveLast = 0;
uint32_t com2StandbyLast = 0;
uint32_t com1Active = 0;
uint32_t com1Standby = 0;
uint32_t com2Active = 0;
uint32_t com2Standby = 0;

uint32_t toBCDNumber(int);

uint32_t processNumber(int);

void updateSimConnection(FSUIPC::SimConnectionStatus);

void disconnect();

void readFrequencyVer1();

void readFrequencyVer2();

void processFrequencyData();

DLL_EXPORT [[maybe_unused]] Version *OpenFSUIPCClient() {
    auto *res = new Version();
    if (!client.open()) {
        res->errMessage = client.getLastErrorMessage();
        return res;
    }
    res->requestStatus = true;
    updateSimConnection(FSUIPC::CONNECTED);
    apiVersion = client.getApiVersion();
    res->apiVersion = apiVersion;
    FSUIPC::VersionInfo version{};
    if (!client.getVersion(version)) {
        res->errMessage = client.getLastErrorMessage();
        return res;
    }
    res->simulatorType = version.simulator;
    res->fsuipcVersion = version.fsuipc;
    return res;
}

DLL_EXPORT [[maybe_unused]] BaseModel *CloseFSUIPCClient() {
    auto *res = new BaseModel();
    disconnect();
    if (client.getLastError() == FSUIPC::Error::OK) {
        res->requestStatus = true;
        return res;
    }
    res->errMessage = client.getLastErrorMessage();
    return res;
}

DLL_EXPORT [[maybe_unused]] Frequencies *ReadFrequencyInfo() {
    auto *res = new Frequencies();
    if (status != FSUIPC::CONNECTED) {
        res->errMessage = "FSUIPC not connected";
        return res;
    }
    if (apiVersion == FSUIPC::ApiVersion::API_UNKNOWN) {
        res->errMessage = "Unsupported FSUIPC api version";
        return res;
    }
    client.readBYTE(radioSwitch);
    if (apiVersion == FSUIPC::ApiVersion::API_VER1) {
        readFrequencyVer1();
    } else {
        readFrequencyVer2();
    }
    processFrequencyData();

    res->frequency[0] = com1Active;
    res->frequency[1] = com1Standby;
    res->frequency[2] = com2Active;
    res->frequency[3] = com2Standby;

    res->frequencyFlag = radioSwitch.data;

    if (client.getLastError() == FSUIPC::Error::OK) {
        res->requestStatus = true;
        return res;
    }
    res->errMessage = client.getLastErrorMessage();
    return res;
}

DLL_EXPORT [[maybe_unused]] ConnectionStatus *GetConnectionState() {
    auto *res = new ConnectionStatus();
    res->requestStatus = true;
    res->status = status;
    return res;
}

DLL_EXPORT [[maybe_unused]] Version *GetFSUIPCVersionInfo() {
    auto *res = new Version();
    if (status != FSUIPC::CONNECTED) {
        res->errMessage = "FSUIPC not connected";
        return res;
    }
    FSUIPC::VersionInfo version{};
    if (!client.getVersion(version)) {
        res->errMessage = client.getLastErrorMessage();
        return res;
    }
    res->apiVersion = apiVersion;
    res->simulatorType = version.simulator;
    res->fsuipcVersion = version.fsuipc;
    return res;
}

DLL_EXPORT [[maybe_unused]] BaseModel *SetCom1Frequency(int frequency) {
    auto *res = new BaseModel();
    if (status != FSUIPC::CONNECTED) {
        res->errMessage = "FSUIPC not connected";
        return res;
    }
    switch (apiVersion) {
        case FSUIPC::ApiVersion::API_UNKNOWN:
            res->errMessage = "Unsupported FSUIPC api version";
            return res;
        case FSUIPC::ApiVersion::API_VER1:
            if (frequency > 100000) {
                frequency -= 100000;
            }
            com1Ver1.data = toBCDNumber(frequency / 10);
            client.writeWORD(com1Ver1);
            break;
        case FSUIPC::ApiVersion::API_VER2:
            com1Ver2.data = frequency * 1000;
            client.writeDWORD(com1Ver2);
            break;
    }
    client.process();
    if (client.getLastError() == FSUIPC::Error::OK) {
        res->requestStatus = true;
        return res;
    }
    res->errMessage = client.getLastErrorMessage();
    return res;
}

DLL_EXPORT [[maybe_unused]] BaseModel *SetCom2Frequency(int frequency) {
    auto *res = new BaseModel();
    if (status != FSUIPC::CONNECTED) {
        res->errMessage = "FSUIPC not connected";
        return res;
    }
    switch (apiVersion) {
        case FSUIPC::ApiVersion::API_UNKNOWN:
            res->errMessage = "Unsupported FSUIPC api version";
            return res;
        case FSUIPC::ApiVersion::API_VER1:
            if (frequency > 100000) {
                frequency -= 100000;
            }
            frequency /= 10;
            com2Ver1.data = toBCDNumber(frequency);
            client.writeWORD(com2Ver1);
            break;
        case FSUIPC::ApiVersion::API_VER2:
            com2Ver2.data = frequency * 1000;
            client.writeDWORD(com2Ver2);
            break;
    }
    client.process();
    if (client.getLastError() == FSUIPC::Error::OK) {
        res->requestStatus = true;
        return res;
    }
    res->errMessage = client.getLastErrorMessage();
    return res;
}

DLL_EXPORT [[maybe_unused]] void FreeMemory(BaseModel *pointer) {
    delete pointer;
}

uint32_t toBCDNumber(int n) {
    uint32_t bcd = 0;
    uint8_t shift = 0;

    while (n > 0) {
        uint8_t digit = n % 10;
        bcd |= (digit << (shift * 4));
        n /= 10;
        shift++;
    }

    return bcd;
}

uint32_t processNumber(int n) {
    std::stringstream ss;
    ss << std::hex << n;
    std::string hexStr = ss.str();

    uint32_t converted = stol(hexStr);

    return (uint32_t) (converted * 10 + 100000 + (converted % 5) * 2.5) * 1000;
}

void updateSimConnection(FSUIPC::SimConnectionStatus connectionStatus) {
    status = connectionStatus;
}

void disconnect() {
    if (status == FSUIPC::CONNECTED) {
        client.close();
        apiVersion = FSUIPC::ApiVersion::API_UNKNOWN;
        updateSimConnection(FSUIPC::NO_CONNECTION);
    }
}

void readFrequencyVer1() {
    client.readWORD(com1ActiveVer1);
    client.readWORD(com1StandbyVer1);
    client.readWORD(com2ActiveVer1);
    client.readWORD(com2StandbyVer1);
    client.process();
    com1Active = processNumber(com1ActiveVer1.data);
    com1Standby = processNumber(com1StandbyVer1.data);
    com2Active = processNumber(com2ActiveVer1.data);
    com2Standby = processNumber(com2StandbyVer1.data);
}

void readFrequencyVer2() {
    client.readDWORD(com1ActiveVer2);
    client.readDWORD(com1StandbyVer2);
    client.readDWORD(com2ActiveVer2);
    client.readDWORD(com2StandbyVer2);
    client.process();
    com1Active = com1ActiveVer2.data;
    com1Standby = com1StandbyVer2.data;
    com2Active = com2ActiveVer2.data;
    com2Standby = com2StandbyVer2.data;
}

void processFrequencyData() {
    if (com1Active == 0 && com1Standby == 0 && com2Active == 0 && com2Standby == 0) {
        com1ActiveLast = com1StandbyLast = com2ActiveLast = com2StandbyLast = 0;
        disconnect();
        return;
    }
    if (com1Active != com1ActiveLast) { com1ActiveLast = com1Active; }
    if (com1Standby != com1StandbyLast) { com1StandbyLast = com1Standby; }
    if (com2Active != com2ActiveLast) { com2ActiveLast = com2Active; }
    if (com2Standby != com2StandbyLast) { com2StandbyLast = com2Standby; }
}
