// Copyright (c) 2025-2026 Half_nothing MIT License

#pragma once

#include "fsuipc_definition.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

struct BaseModel {
    bool requestStatus{false};
    const char *errMessage{"No error found"};
};

struct ConnectionStatus : public BaseModel {
    uint32_t status{FSUIPC::SimConnectionStatus::NO_CONNECTION};
};

struct Frequencies : public BaseModel {
    uint8_t frequencyFlag{};
    uint32_t frequency[4]{};
};

struct Version : public BaseModel {
    uint16_t simulatorType{};
    uint32_t fsuipcVersion{};
    uint8_t apiVersion{};
};

DLL_EXPORT Version *OpenFSUIPCClient();
DLL_EXPORT BaseModel *CloseFSUIPCClient();
DLL_EXPORT Frequencies *ReadFrequencyInfo();
DLL_EXPORT ConnectionStatus *GetConnectionState();
DLL_EXPORT Version *GetFSUIPCVersionInfo();
DLL_EXPORT BaseModel *SetCom1Frequency(int);
DLL_EXPORT BaseModel *SetCom2Frequency(int);
DLL_EXPORT void FreeMemory(BaseModel *);
