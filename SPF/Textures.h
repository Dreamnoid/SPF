#include "Core.h"

struct Texture
{
	bool InUse;
	HardwareID GLID;
	unsigned int Width;
	unsigned int Height;
	bool Flipped;
};

constexpr uint32_t TexturesCount = 200;

class Textures
{
private:
	Texture mTextures[TexturesCount];

public:
	ResourceIndex Create(int w, int h, void* pixels, bool flipped);
	ResourceIndex Load(unsigned char* buffer, int length);
	void Delete(ResourceIndex texture);
	int GetWidth(ResourceIndex texture);
	int GetHeight(ResourceIndex texture);
	void SetFiltering(ResourceIndex texture, bool filtering);

	inline const Texture& Get(ResourceIndex texture) const
	{
		return mTextures[texture];
	}
};

extern Textures mTextures;

