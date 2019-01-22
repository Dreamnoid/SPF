#include "FS.h"
#include <stdlib.h>
#include <stdio.h>
#include <physfs.h>

void FS_Init()
{
	PHYSFS_init(0);
}

void FS_AddToSearchPath(const char* filename)
{
	PHYSFS_addToSearchPath(filename, 1);
}

Buffer FS_LoadBuffer(const char* filename)
{
	PHYSFS_file* file = PHYSFS_openRead(filename);
	PHYSFS_sint64 file_size = PHYSFS_fileLength(file);
	char* buffer = malloc(file_size);
	PHYSFS_sint64 length_read = PHYSFS_read(file, buffer, 1, file_size);
	PHYSFS_close(file);

	Buffer result;
	result.Start = (void*)buffer;
	result.Length = (unsigned int)length_read;
	return result;
}

void FS_FreeBuffer(Buffer* buffer)
{
	free(buffer->Start);
	buffer->Start = 0;
	buffer->Length = 0;
}