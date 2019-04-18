#pragma once
#include <cstdint>

using ResourceIndex = int32_t;
using HardwareID = uint32_t;

#define DLLExport __declspec(dllexport)

void FatalError(const char* msg);