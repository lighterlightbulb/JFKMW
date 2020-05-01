#pragma once
#define tile_table_size 14

#define tile_1 0
#define tile_2 2
#define tile_3 4
#define tile_4 6
#define tile_palette_1 8
#define tile_palette_2 9
#define tile_flips 10
#define collision 11
#define act_as_high 12
#define act_as_low 13
#define ram_level_low 0x8000
#define ram_level_high 0xC000


uint8_t map16_entries[tile_table_size * 0xFFFF]; //65535 entries.

void reset_map()
{
	for (int i = ram_level_low; i < ram_level_high; i++)
	{
		ServerRAM.RAM[i] = 0x25;
		ServerRAM.RAM[i + 0x4000] = 0x00;

	}

	for (int i = 0; i < 0x1000; i++)
	{
		ServerRAM.RAM[0x2000 + i] = 0;
	}
}
void initialize_map16()
{
	string file = path + "Map16/Global.Cmap16";

	cout << blue << "[MAP16] Loading " << file << white << endl;
	ifstream input(file, ios::binary);
	vector<unsigned char> buffer(istreambuf_iterator<char>(input), {});


	uint8_t temp[16];
	int current_byte = 0;
	for (auto &v : buffer)
	{
		temp[current_byte] = uint8_t(v);
		current_byte += 1;
		if (current_byte >= 16) {
			uint16_t replace_p = temp[1] + temp[0] * 256; //this is actually a thing.

			cout << blue << "[MAP16] Initialized tile " << hex << replace_p << " of properties ";
			for (int i = 0; i < tile_table_size; i++)
			{
				map16_entries[(replace_p * tile_table_size) + i] = temp[i + 2];
				cout << hex << int(temp[i + 2]);
			}

			cout << " - ";
			int integer = map16_entries[collision + replace_p * tile_table_size];
			for (int i = 0; i < 8; i++)
			{
				cout << ((integer >> (7-i)) & 1);
			}

			current_byte = 0;
			cout << white << endl;
			
		}
	}
	//reset_map();


}


//$6000-$8000 High Byte Map16
//$4000-$6000 Low Byte Map16

class map16blockhandler //Map16 loaded block
{
public:
	int tile;
	int act_as;
	bool logic[8];

	void get_map_16_details()
	{
		int entry = tile * tile_table_size;
		act_as = map16_entries[entry + act_as_low] + map16_entries[entry + act_as_high] * 256;

		int integer = map16_entries[entry + collision];
		for (int i = 0; i < 8; i++)
		{
			logic[i] = ((integer >> (7-i)) & 1) != 0;
		}
	}

	/*
		Update Map Tile
	*/
	void update_map_tile(uint_fast16_t x, uint_fast16_t y)
	{
		uint_fast32_t index = (x % mapWidth) + (y * mapWidth);
		tile = ServerRAM.RAM[ram_level_low + index] + ServerRAM.RAM[ram_level_high + index] * 256;
		get_map_16_details();
	}

	/*
		Replace Map tile with anything
	*/
	void replace_map_tile(uint16_t tile, uint_fast16_t x, uint_fast16_t y)
	{
		uint_fast32_t index = (x % mapWidth) + (y * mapWidth);
		ServerRAM.RAM[ram_level_low + index] = tile % 256; ServerRAM.RAM[ram_level_high + index] = tile / 256;
	}

	/*
		Process block hit.
	*/
	void process_block(uint_fast16_t x, uint_fast16_t y, uint8_t side, bool pressing_y = false)
	{
		if (!networking || !isClient)
		{
			uint_fast32_t index = (x % mapWidth) + (y * mapWidth);


			uint_fast16_t t = ServerRAM.RAM[ram_level_low + index] + ServerRAM.RAM[ram_level_high + index] * 256;
			if (t == 0x0124 && side == bottom)
			{
				ServerRAM.RAM[ram_level_low + index] = 0x32;
			}
			if (t == 0x002B)
			{
				replace_map_tile(0x0025, x, y);
				ServerRAM.RAM[0x1DFC] = 1;
			}

			if (t == 0x012E && pressing_y)
			{
				replace_map_tile(0x0025, x, y);
				x *= 16;
				y *= 16;
				spawnSpriteJFKMarioWorld(0x53, 2, x, y, 0, true);

			}
		}

	}

	/*
		Get tile on map.
	*/
	uint_fast16_t get_tile(uint_fast16_t x, uint_fast16_t y)
	{
		uint_fast32_t index = (x % mapWidth) + (y * mapWidth);
		return ServerRAM.RAM[ram_level_low + index] + ServerRAM.RAM[ram_level_high + index] * 256;
	}


};

map16blockhandler map16_handler;