#define TEXTURES_COUNT 200
#define SURFACES_COUNT 20
#define SOUNDS_COUNT 100

typedef struct
{
	float X;
	float Y;
	float U;
	float V;
	float R;
	float G;
	float B;
	float A;
} Vertex;

#define MAX_SPRITES 2000
#define VERTICES_PER_SPRITE 4
#define VERTICES_COUNT MAX_SPRITES * VERTICES_PER_SPRITE

typedef struct
{
	bool InUse;
	SDL_AudioSpec Spec;
	Uint8* Buffer;
	Uint32 Length;
} Sound;


// All the data stored by the platform layer itself
struct
{
	struct
	{
		bool InUse;
		GLuint GLID;
		unsigned int Width;
		unsigned int Height;
		bool Flipped;
	} Textures[TEXTURES_COUNT];

	struct
	{
		bool InUse;
		GLuint GLID;
		ResourceIndex Texture;
	} Surfaces[SURFACES_COUNT];

	int WindowWidth;
	int WindowHeight;
	SDL_GLContext OpenGLContext;
	SDL_Window* Window;
	unsigned int LastTick;

	int KeysDownPreviousFrame[SDL_NUM_SCANCODES];
	int KeysDown[SDL_NUM_SCANCODES];

	int ButtonsDownPreviousFrame[SDL_CONTROLLER_BUTTON_MAX];
	int ButtonsDown[SDL_CONTROLLER_BUTTON_MAX];

	Vertex Vertices[VERTICES_COUNT];
	struct
	{
		int VertexCount;
		GLuint CurrentTexture;
	} BatchInfo;

	GLuint BatchVBOID;
	GLuint EmptyTexture;
	GLuint Program;

	int CurrentWidth;
	int CurrentHeight;

	Sound Sounds[SOUNDS_COUNT];

	SDL_AudioSpec SFXPlaybackSpecs;
	SDL_AudioDeviceID SFXDevice;

	SDL_GameController* Controller;

} Data;

void InitData()
{
	memset(&Data, 0, sizeof(Data));
}