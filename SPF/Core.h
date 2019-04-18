#pragma once
#include <cstdint>

using ResourceIndex = int32_t;
using HardwareID = uint32_t;

#define DLLExport __declspec(dllexport)

void FatalError(const char* msg);

struct Vertex
{
	float X, Y, Z;
	float U, V, BU, BV;
	float R, G, B, A;
	float OverlayR, OverlayG, OverlayB, OverlayA;
};