#pragma once

extern void FS_Init();
extern void FS_AddToSearchPath(const char* filename);

typedef struct
{
	void* Start;
	unsigned int Length;
} Buffer;

extern Buffer FS_LoadBuffer(const char* filename);
extern void FS_FreeBuffer(Buffer* buffer);