#pragma once

SDL_Surface* MAP16_SURF;
SDL_Texture* MAP16_TEX;

void readBMP(char* filename) {
	uint_fast32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	MAP16_SURF = SDL_CreateRGBSurface(0, 1024, 512, 32,
		rmask, gmask, bmask, amask);
	FILE* f = fopen(filename, "rb");

	//std::cout << "Putting into array" << std::endl;
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	int width = *(int*)& info[18];
	int height = *(int*)& info[22];

	int size = 3 * width * height;
	uint_fast8_t* data = new uint_fast8_t[0x200000]; // allocate 3 bytes per pixel
	fread(data, sizeof(uint_fast8_t), size, f); // read the rest of the data at once
	fclose(f);

	//std::cout << "Now reading.. Size : " << width << ", " << height << std::endl;
	uint_fast64_t index = 0;
	for (uint_fast64_t j = 0; j < height; j++) {
		for (uint_fast64_t k = 0; k < width; k++) {
			index = ((j * width) * 3) + k * 3;

			uint_fast8_t r = data[index + 2];
			uint_fast8_t g = data[index + 1];
			uint_fast8_t b = data[index];

			if (r == 0 && g == 255 && b == 255)
			{
				continue;
			}
			else
			{
				Uint32* p_screen = (Uint32*)(MAP16_SURF)->pixels + (k + (511 - j) * 1024);
				*p_screen = r + (g << 8) + (b << 16) + 0xFF000000;

				r = 255 - r; g = 255 - g; b = 255 - b;
				p_screen = (Uint32*)(MAP16_SURF)->pixels + (512 + k + (511 - j) * 1024);
				*p_screen = r + (g << 8) + (b << 16) + 0xFF000000;
			}

		}
	}
	delete[]data;

	SDL_LockSurface(MAP16_SURF);
	MAP16_TEX = SDL_CreateTextureFromSurface(ren, MAP16_SURF);
}

void InitializeMap16()
{
	cout << "[SE] Loading assets..." << endl;
	std::string str = path + "map16data.bmp";
	readBMP((char*)str.c_str());
}