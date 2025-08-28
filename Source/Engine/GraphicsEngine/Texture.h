#pragma once

enum TextureWrapMode
{
	Repeat = 0x2901,
	RepeatMirror = 0x8370,
	ClampToEdge= 0x812F,
	ClampToBorder = 0x812D
};

enum SamplingType
{
	Point = 0x2600,
	Bilinear = 0x2601,
};

struct Texture
{
	int width;
	int height;
	int nrChannels;

	unsigned myID = static_cast<unsigned>(-1);
	char* myPath;
};
