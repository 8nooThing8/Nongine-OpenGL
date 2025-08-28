#pragma once

#include <vector>
#include <fstream>

#define SKIP_BYTES(mem, byte_num) (mem += byte_num);
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int32_t int32;

namespace Non
{
	struct Texture
	{
		unsigned width;
		unsigned height;
		unsigned depth;
		unsigned colorType;
		unsigned compressionMethod;
		unsigned filterMethod;
		unsigned interlaceMethod;

		std::vector<uint8_t> outTexture;
	};

	namespace TextureLoaderInternal
	{
		struct sh_png_chunk
		{
			uint32 data_length;
			uint8 type[4];
			uint8* data;
			uint32 crc32;
		};

		uint8* sh_memalloc(uint32 bytes_to_allocate) {
			return (uint8*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes_to_allocate);
		}

		uint8 sh_memfree(uint8* mem_pointer) {
			return HeapFree(GetProcessHeap(), 0, (LPVOID)mem_pointer);
		}

		void sh_memcpy(uint8* from, uint8* to, uint32 bytes_to_copy) { //copy some bytes from "from" to "to" 

			while (bytes_to_copy-- > 0) {
				*(to + bytes_to_copy) = *(from + bytes_to_copy);
			}
		}

		void sh_memset(uint8* mem, uint8 value_to_use, uint32 bytes_to_set) {
			while (bytes_to_set-- > 0) {
				*mem++ = value_to_use;
			}
		}

		uint16 sh_get_uint16be(uint8* mem) {
			uint16 result = 0;
			for (uint32 i = 0; i < 2; ++i) {
				result <<= 8;
				result |= *(mem + i);
			}

			return result;
		}


		uint32 sh_get_uint32be(uint8* mem) {
			uint32 result = 0;
			for (uint32 i = 0; i < 4; ++i) {
				result <<= 8;
				result |= *(mem + i);
			}

			return result;
		}

		sh_png_chunk sh_png_read_chunk(uint8* mem) 
		{
			sh_png_chunk chunk = {};
			chunk.data_length = sh_get_uint32be(mem);
			SKIP_BYTES(mem, 4); //we move 4 bytes over because we read the length, which is 4 bytes

			*((uint32*)&chunk.type) = *((uint32*)mem);
			SKIP_BYTES(mem, 4);

			chunk.data = sh_memalloc(chunk.data_length);

			sh_memcpy(mem, chunk.data, chunk.data_length);
			SKIP_BYTES(mem, chunk.data_length);

			chunk.crc32 = sh_get_uint32be(mem);

			return chunk;
		}

		void HandlePNG(Texture& aTexture)
		{
			sh_png_read_chunk(aTexture.outTexture.data());
		}
	}

	static bool LoadTexture(const char* aPath, Texture& aTexture)
	{
		std::ifstream fileStream(aPath, std::ios::binary);

		if (!fileStream.is_open())
			return false;

		fileStream.seekg(0, std::ios::end);
		size_t size = fileStream.tellg();
		fileStream.seekg(0, std::ios::beg);

		aTexture.outTexture.resize(size);

		fileStream.read((char*)aTexture.outTexture.data(), size);
		fileStream.close();

		if (!memcmp(&aTexture.outTexture[1], "PNG", 3))
		{
			aTexture.outTexture.erase(aTexture.outTexture.begin(), aTexture.outTexture.begin() + 12);

			TextureLoaderInternal::HandlePNG(aTexture);
		}
	}
}