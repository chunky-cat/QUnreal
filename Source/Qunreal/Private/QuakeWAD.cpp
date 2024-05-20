#include "QuakeWAD.h"

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

enum WadEntryType
{
	Palette = 0x40,
	SBarPic = 0x42,
	MipsTexture = 0x44,
	ConsolePic = 0x45
};

struct WadEntry
{
	uint offset;
	uint inWadSize;
	uint size;
	u_char type;
	u_char compression;
	ushort unknown;
	std::string nameStr;
};

struct TextureData
{
	char* name;
	uint width;
	uint height;
	u_char *pixelData;

	TextureData(char* name, uint width, uint height, u_char *pixelData)
	{
		name = name;
		width = width;
		height = height;
		pixelData = pixelData;
	}
};

constexpr int TEXTURE_NAME_LENGTH = 16;
constexpr int MAX_MIP_LEVELS = 4;

void UQuakeWAD::ProcessFile(FString sourceFile)
{
	SourceQMapFile = sourceFile;
}
