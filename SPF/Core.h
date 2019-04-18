#pragma once
#include <cstdint>

using ResourceIndex = int32_t;
using HardwareID = uint32_t;

#define DLLExport __declspec(dllexport)

void FatalError(const char* msg);

struct Vertex
{
	float X;
	float Y;
	float U;
	float V;
	float R;
	float G;
	float B;
	float A;
	float OverlayR;
	float OverlayG;
	float OverlayB;
	float OverlayA;
};
