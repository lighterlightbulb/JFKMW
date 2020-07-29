#pragma once

struct Color {
	uint_fast8_t r, g, b;
};

Color MAP16_IMG[520][520];

void readBMP(char* filename) {
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

			MAP16_IMG[1 + k][512 - j] = Color{ r, g, b };
		}
	}
	delete[]data;
}

void InitializeMap16()
{
	cout << "[SE] Loading assets..." << endl;
	std::string str = path + "map16data.bmp";
	readBMP((char*)str.c_str());
}