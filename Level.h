#pragma once



class Level
{
public:
	Level() {}


	//std::vector<string, int> data;
	std::string current_level;
	std::unordered_map<std::string, uint_fast32_t> level_data;

	uint_fast32_t request_level_entry(std::string name)
	{
		auto entry = level_data.find(name);

		if (entry != level_data.end())
		{
			return entry->second;
		}

		std::cout << green
			<< "[Level Manager] Couldn't find entry " << name
			<< " : not created yet?" << white << std::endl;

		return 0;
	}

	void add_entry(std::string name, uint_fast32_t value)
	{

		std::cout << green
			<< "[Level Manager] Added entry " << name
			<< " = " << dec << value << white << std::endl;


		level_data.insert(std::make_pair(name, value));
	}


	std::string LoadLevelData(std::string FILENAME)
	{
		std::ifstream file;
		file.open(FILENAME);
		if (file.is_open())
		{

			std::stringstream strStream;
			strStream << file.rdbuf(); //read the file
			std::string str = strStream.str(); //str holds the content of the file

			return str;
		}
		file.close();
		return "Error";
	}
	void LoadLevelFromFile(std::string FILENAME, uint_fast16_t num)
	{
		current_level = FILENAME;
		std::cout << green
			<< "[Level Manager] Loading " << FILENAME
			<< white << std::endl;
		LoadLevelFromString(LoadLevelData(FILENAME), num);
	}

	void LoadLevelFromString(std::string DLevel, uint_fast16_t num)
	{
		
		reset_map();

		std::cout << green
			<< "[Level Manager] Processing level string.."
			<< white << std::endl;

		std::string status;
		std::string line;

		istringstream str(DLevel); // std::string

		int spr_index = 0;
		while (getline(str, line)) {
			// using printf() in all tests for consistency
			if (line != "")
			{
				//cout << line << endl;
				string CHECK = line.substr(0, 2);

				if (CHECK != "//")
				{
					if (line.substr(0, 1) == "[")
					{
						status = line.substr(1, line.length() - 2);
						std::cout << green << "[Level Manager] Status = " << status << white << std::endl;

						uint_fast32_t size[2] = {
							request_level_entry("size_x"),
							request_level_entry("size_y")
						};

						ASM.Write_To_Ram(0x3F00, size[0], 2);
						ASM.Write_To_Ram(0x3F02, size[1], 2);
						mapWidth = size[0];
						mapHeight = size[1];

						//data.push_back();
						continue;
					}

					if(status == "level_config")
					{
						line.erase(std::remove_if(line.begin(), line.end(), isspace),
							line.end());
						auto delimiterPos = line.find("=");
						auto name = line.substr(0, delimiterPos);
						auto value = line.substr(delimiterPos + 1);

						// std::cout << green << "[Level Manager] adding level config entry " << name << " = " << value << white << std::endl;
						add_entry(name, stoi(value));
					}

					if (status == "scripts")
					{
						line.erase(std::remove_if(line.begin(), line.end(), isspace),
							line.end());
						auto delimiterPos = line.find("=");
						auto name = line.substr(0, delimiterPos);
						auto value = line.substr(delimiterPos + 1);
						auto type = name.substr(name.length() - 3);

						//std::cout << "[load script] type " << type << std::endl;
						std::cout << green
							<< "[Level Manager] Loading Script " << name << " of type " << type
							<< white << std::endl;
						// std::cout << green << "[Level Manager] adding level config entry " << name << " = " << value << white << std::endl;
						if (type == "lua")
						{
							lua_loadfile("Levels/" + int_to_hex(num) + "/" + name);
							lua_loaded = true;
							lua_run_init();
						}

						if (type == "asm")
						{
							ASM.load_asm("Levels/" + int_to_hex(num) + "/" + name, location_rom_levelasm);
							if (value == "init")
							{
								ASM.start_JFK_thread(location_rom_levelasm);
							}
							else
							{
								asm_loaded = true;
							}
						}

					}

					if (status == "level_data")
					{
						//cout << "Level loading Line = " + line << endl;
						vector<string> v = split(line.c_str(), ',');

						int x_start, x_end, y_start, y_end;
						int tile = 0x0;


						int dataType = 0;
						string data;
						for (auto i : v)
						{
							if (dataType <= 5)
							{
								//cout << "loaded " + i << endl;
								//cout << dataType << endl;
								if (dataType == 0)
								{
									tile = std::stoi(i, nullptr, 16);
								}
								if (dataType == 1) { x_start = std::stoi(i); }
								if (dataType == 2) { y_start = std::stoi(i); }
								if (dataType == 3) { x_end = std::stoi(i); }
								if (dataType == 4) { y_end = std::stoi(i); }

								dataType += 1;
							}
						}


						//cout << "Data as follows \nActs as : " + data[1] + "\nTile : " + data[2] + "\nStart and End (X) : " + data[3] + "-" + data[5] + "\nStart and End (Y) : " + data[4] + "-" + data[6] + "\n";

						for (int x = x_start; x <= x_end; x++)
						{
							for (int y = y_start; y <= y_end; y++)
							{
								map16_handler.replace_map_tile(tile, x, y);
							}
						}
					}

					if (status == "sprite_data")
					{
						vector<string> v = split(line.c_str(), ',');

						sprite_is_lua[spr_index] = v[0] == "lua";
						
						ServerRAM.RAM[0x2000 + spr_index] = 1;
						ServerRAM.RAM[0x2080 + spr_index] = stoi(v[1], nullptr, 16);
						ServerRAM.RAM[0x2100 + spr_index] = stoi(v[2]) % 256;
						ServerRAM.RAM[0x2180 + spr_index] = stoi(v[2]) / 256;
						ServerRAM.RAM[0x2200 + spr_index] = 0;

						ServerRAM.RAM[0x2280 + spr_index] = stoi(v[3]) % 256;
						ServerRAM.RAM[0x2300 + spr_index] = stoi(v[3]) / 256;
						ServerRAM.RAM[0x2380 + spr_index] = 0;

						ServerRAM.RAM[0x2680 + spr_index] = stoi(v[4]);
						ServerRAM.RAM[0x2F80 + spr_index] = 0;

						spr_index += 1;
					}

				}
			}

		}

		std::cout << green
			<< "[Level Manager] Finished loading level."
			<< white << std::endl;
	}

	void Initialize_Level()
	{
		ASM.Write_To_Ram(0x1DFB, request_level_entry("music"), 1);
		ASM.Write_To_Ram(0x3F05, request_level_entry("background"), 1);

		recent_big_change = true;
		Set_Server_RAM();
	}

	void LoadLevel(uint_fast16_t num)
	{

		while (!level_data.empty())
		{
			level_data.erase(level_data.begin());
		}

		std::cout << green
			<< "[Level Manager] Loading level " << int_to_hex(num) << ".."
			<< white << std::endl;

		read_from_palette(path + "Levels/" + int_to_hex(num) + "/level_palette.mw3");
		LoadLevelFromFile(path + "Levels/" + int_to_hex(num) + "/level_data.txt", num);

		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_1"), true) + ".bin", 0); //FG1
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_2"), true) + ".bin", 1); //FG2
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_3"), true) + ".bin", 2); //FG3
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_4"), true) + ".bin", 3); //FG4
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_5"), true) + ".bin", 4); //FG5
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_6"), true) + ".bin", 5); //FG6
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_7"), true) + ".bin", 6); //FG7
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("gfx_8"), true) + ".bin", 7); //FG8

		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("sp_1"), true) + ".bin", 12); //SP1
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("sp_2"), true) + ".bin", 13); //SP2
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("sp_3"), true) + ".bin", 14); //SP3
		decode_graphics_file("Graphics/GFX" + int_to_hex(request_level_entry("sp_4"), true) + ".bin", 15); //SP4


		Initialize_Level();
	}
};

Level LevelManager;