ResourceIndex CreateTexture(int w, int h, GLvoid* pixels, bool flipped)
{
	GLuint ids[1];
	glGenTextures(1, ids);
	GLuint id = ids[0];

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	ResourceIndex texID = 0;
	for (ResourceIndex texID = 0; texID < TEXTURES_COUNT; ++texID)
	{
		if (!Data.Textures[texID].InUse)
		{
			Data.Textures[texID].GLID = id;
			Data.Textures[texID].Width = w;
			Data.Textures[texID].Height = h;
			Data.Textures[texID].InUse = 1;
			Data.Textures[texID].Flipped = flipped;
			return texID;
		}
	}
	exit(1); // All slots used
}

DLLExport ResourceIndex LoadTexture(const char* filename)
{
	int w, h, bpp;
	stbi_uc* pixels = stbi_load(filename, &w, &h, &bpp, 4);
	ResourceIndex index = CreateTexture(w, h, (GLvoid*)pixels, 0);
	stbi_image_free((void*)pixels);
	return index;
}

DLLExport void DeleteTexture(ResourceIndex texture)
{
	Data.Textures[texture].InUse = 0;
	GLuint ids[1];
	ids[0] = Data.Textures[texture].GLID;
	glDeleteTextures(1, ids);
}

DLLExport int GetTextureWidth(ResourceIndex texture)
{
	return Data.Textures[texture].Width;
}

DLLExport int GetTextureHeight(ResourceIndex texture)
{
	return Data.Textures[texture].Height;
}

DLLExport ResourceIndex CreateSurface(int w, int h)
{
	ResourceIndex texture = CreateTexture(w, h, NULL, 1);

	GLuint ids[1];
	glGenFramebuffers(1, ids);
	GLuint fboID = ids[0];
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Data.Textures[texture].GLID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ResourceIndex surface = 0;
	for (ResourceIndex surfaceID = 0; surfaceID < SURFACES_COUNT; ++surfaceID)
	{
		if (!Data.Surfaces[surfaceID].InUse)
		{
			Data.Surfaces[surfaceID].GLID = fboID;
			Data.Surfaces[surfaceID].Texture = texture;
			Data.Surfaces[surfaceID].InUse = 1;
			return surfaceID;
		}
	}
	exit(1); // All slots used
}

void IssueVertices();

DLLExport void BeginSurface(ResourceIndex surface)
{
	ResourceIndex texture = Data.Surfaces[surface].Texture;
	glBindFramebuffer(GL_FRAMEBUFFER, Data.Surfaces[surface].GLID);
	Data.CurrentWidth = Data.Textures[texture].Width;
	Data.CurrentHeight = Data.Textures[texture].Height;
	glViewport(0, 0, (GLsizei)Data.CurrentWidth, (GLsizei)Data.CurrentHeight);
	glClear(GL_COLOR_BUFFER_BIT);
}

DLLExport void EndSurface()
{
	IssueVertices();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Data.CurrentWidth = Data.WindowWidth;
	Data.CurrentHeight = Data.WindowHeight;
	glViewport(0, 0, (GLsizei)Data.CurrentWidth, (GLsizei)Data.CurrentHeight);
	glClear(GL_COLOR_BUFFER_BIT);
}

DLLExport void DeleteSurface(ResourceIndex surface)
{
	Data.Surfaces[surface].InUse = 0;
	GLuint ids[1];
	ids[0] = Data.Surfaces[surface].GLID;
	glDeleteFramebuffers(1, ids);
}

DLLExport ResourceIndex GetSurfaceTexture(ResourceIndex surface)
{
	return Data.Surfaces[surface].Texture;
}