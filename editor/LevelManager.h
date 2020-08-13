#pragma once
vector<string> split(const string& s, char delim) {
	vector<string> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

class installedSprite {
public:
	uint_fast8_t num;
	string name;
};
vector<installedSprite> installedSprites;

void InitInstalledSprites()
{
	cout << yellow << "[SE] Loading sprite lists.." << endl;
	installedSprites.clear();
	installedSprites.push_back({ 0x00, "Shell-less Green Koopa" });
	installedSprites.push_back({ 0x01, "Shell-less Red Koopa" });
	installedSprites.push_back({ 0x04, "Green Koopa" });
	installedSprites.push_back({ 0x05, "Red Koopa" });
	installedSprites.push_back({ 0x09, "Bouncing Green Koopa" });
	installedSprites.push_back({ 0x0A, "Red vertical Parakoopa" });
	installedSprites.push_back({ 0x0B, "Red horizontal Parakoopa" });
	installedSprites.push_back({ 0x1C, "Projectile Bullet Bill" });
	installedSprites.push_back({ 0x4A, "Goal Point Question Sphere" });
	installedSprites.push_back({ 0x53, "Throwblock" });
	installedSprites.push_back({ 0x55, "Grey Moving Platform (Horizontal)" });
	installedSprites.push_back({ 0x57, "Grey Moving Platform (Vertical)" });
	installedSprites.push_back({ 0x74, "Mushroom" });
	installedSprites.push_back({ 0x9E, "Ball n chain" });
	installedSprites.push_back({ 0x9F, "Banzai Bill" });
	installedSprites.push_back({ 0xA5, "Ground Guided Fuzzy" });
	installedSprites.push_back({ 0xAB, "Rex" });
	installedSprites.push_back({ 0xB6, "Reflecting Fireball" });
	installedSprites.push_back({ 0xBA, "Timed Platform (4)" });
	installedSprites.push_back({ 0xC4, "Grey Falling Platform" });
	installedSprites.push_back({ 0xC9, "Projectile Bullet Bill Shooter" });
	installedSprites.push_back({ 0xDA, "Kickable Green Shell" });
	installedSprites.push_back({ 0xDB, "Kickable Red Shell" });
	installedSprites.push_back({ 0xFA, "YI Bird" });
	installedSprites.push_back({ 0xFB, "Noob boss" });
	installedSprites.push_back({ 0xFC, "CIA Sonic From We Schlee" });
	installedSprites.push_back({ 0xFD, "Danmaku Clusters" });
	installedSprites.push_back({ 0xFE, "Morsel Bombs" });
	installedSprites.push_back({ 0xFF, "Rapping Morsel" });
	cout << "[SE] Loaded " << installedSprites.size() << " sprites." << white << endl;
}

string int_to_hex(int T, bool add_0 = false)
{
	stringstream stream;
	if (!add_0)
	{
		stream << hex << T;
	}
	else
	{
		if (T < 16)
		{
			stream << "0";
		}
		stream << hex << T;
	}

	return stream.str();
}


class LevelManager
{
public:
	std::string status;
	std::string line;
	std::string current_level;
	std::string scripts;
	std::unordered_map<std::string, uint_fast32_t> level_data;
	int blocks_l;

	LevelManager()
	{
	}

	uint_fast32_t request_level_entry(std::string name)
	{
		auto entry = level_data.find(name);

		if (entry != level_data.end())
		{
			return entry->second;
		}

		return 0;
	}

	void add_entry(std::string name, uint_fast32_t value)
	{
		level_data.insert(std::make_pair(name, value));
	}


	void LoadLevelFromString(std::string DLevel)
	{
		int Curr_Sprite = 0;

		level_data.clear();
		scripts = "";

		istringstream str(DLevel); // std::string
		while (getline(str, line)) {
			// using printf() in all tests for consistency
			if (line != "")
			{
				if (line.substr(0, 1) == "[")
				{
					status = line.substr(1, line.length() - 2);

					continue;
				}

				//cout << line << endl;
				string CHECK = line.substr(0, 2);

				if (CHECK != "//")
				{
					if (status == "scripts")
					{
						scripts = scripts + line + "\n";
					}
					if (status == "sprite_data")
					{
						vector<string> v = split(line.c_str(), ',');
						Sprite& Curr_Spr = Sprite_data[Curr_Sprite];
						Curr_Spr.type = v[0] == "lua";
						Curr_Spr.num = stoi(v[1], nullptr, 16);
						Curr_Spr.x = stoi(v[2], nullptr, 10);
						Curr_Spr.y = stoi(v[3], nullptr, 10);
						Curr_Spr.dir = stoi(v[4], nullptr, 10);
						Curr_Spr.exists = true;
						Curr_Sprite += 1;
					}
					if (status == "level_config")
					{
						line.erase(remove_if(line.begin(), line.end(), ::isspace),
							line.end());
						auto delimiterPos = line.find("=");
						auto name = line.substr(0, delimiterPos);
						auto value = line.substr(delimiterPos + 1);

						if (name == "music" || name == "background")
						{
							add_entry(name, stoi(value, nullptr, 16));
						}
						else
						{
							add_entry(name, stoi(value));
						}
						if (name == "size_x")
						{
							L_SIZE_X = stoi(value);
						}
						if (name == "size_y")
						{
							L_SIZE_Y = stoi(value);
						}
						if (name == "start_x")
						{
							start_x = stoi(value);
						}
						if (name == "start_y")
						{
							start_y = stoi(value);
						}
					}
					if (status == "level_data" && line != status)
					{
						//cout << "Level loading Line = " + line << endl;
						vector<string> v = split(line.c_str(), ',');

						if (v.size() == 5)
						{
							int x = stoi(v[1]);
							int y = stoi(v[2]);
							int w = stoi(v[3]);
							int h = stoi(v[4]);

							for (int i = x; i <= w; i++) {
								for (int e = y; e <= h; e++) {
									leveldata[i + e * L_SIZE_X] = stoi(v[0], nullptr, 16);
								}
							}
						}
						if (v.size() == 3)
						{
							int x = stoi(v[1]);
							int y = stoi(v[2]);
							leveldata[x + y * L_SIZE_X] = stoi(v[0], nullptr, 16);
						}
							


					}
				}
			}
		}
	}
};
LevelManager lManager;
void LoadLevelFromFile(std::string FILENAME)
{
	int size = 1024, pos;
	int c;
	char* buffer = (char*)malloc(size);

	string level_data = "";
	FILE* f = fopen(FILENAME.c_str(), "r");
	if (f) {
		do { // read all lines in file
			pos = 0;
			do { // read one line
				c = fgetc(f);
				if (c != EOF) buffer[pos++] = (char)c;
				if (pos >= size - 1) { // increase buffer length - leave room for 0
					size *= 2;
					buffer = (char*)realloc(buffer, size);
				}
			} while (c != EOF && c != '\r');
			buffer[pos] = 0;
			// line is now in buffer
			level_data = string(buffer) + "\n";
		} while (c != EOF);
		fclose(f);
	}
	free(buffer);

	lManager.LoadLevelFromString(level_data);
}


string ExportLevelToString()
{
	// declaring argument of time() 
	time_t my_time = time(NULL);

	// ctime() used to give the present time 
	cout << ctime(&my_time) << "Starting level save process." << endl;

	int sprites = 0;
	int level_chunks = 0;
	int level_chunks_original = 0;
	int config_entries = 0;
	cout << "Exporting level..." << endl;
	string exported = "[level_config]\n";
	for (auto const& x : lManager.level_data)
	{
		config_entries += 1;
		if (x.first == "music" || x.first == "background") {
			exported = exported + x.first + " = " + int_to_hex(x.second, true) + "\n";
		}
		else {
			if (x.first == "start_x")
			{
				exported = exported + "start_x = " + to_string(start_x) + "\n";
				continue;
			}
			if (x.first == "start_y")
			{
				exported = exported + "start_y = " + to_string(start_y) + "\n";
				continue;
			}
			exported = exported + x.first + " = " + to_string(x.second) + "\n";
		}
	}
	exported = exported + "\n[scripts]\n" + lManager.scripts;
	exported = exported + "\n[sprite_data]\n";
	for (int i = 0; i < 512; i++)
	{
		if (Sprite_data[i].exists && (Sprite_data[i].num >= 0 && Sprite_data[i].num <= 255))
		{
			sprites += 1;
			exported = exported + (Sprite_data[i].type ? "lua" : "asm")
				+ "," + int_to_hex(Sprite_data[i].num, true)
				+ "," + to_string(Sprite_data[i].x)
				+ "," + to_string(Sprite_data[i].y)
				+ "," + to_string(Sprite_data[i].dir)
				+ "\n";
		}
	}

	for (int x = 0; x < 0x4000; x++)
	{
		if (leveldata[x] != 0x25)
		{
			level_chunks_original++;
		}
	}
	exported = exported + "\n[level_data]\n";
	for (int x = 0; x < L_SIZE_X; x++)
	{
		for (int y = 0; y < L_SIZE_Y; y++)
		{

			if (leveldata[x + y * L_SIZE_X] != 0x25)
			{
				exported = exported + int_to_hex(leveldata[x + y * L_SIZE_X]) + "," + to_string(x) + "," + to_string(y) + "\n";
				level_chunks++;
			}
		}
	}
	cout << "Exported : " << endl <<
		sprites << " sprites." << endl << 
		config_entries << " config entries." << endl <<
		level_chunks << " optimized level chunks. (" << level_chunks_original << " originally)" << endl;
	return exported;
}


void LoadFile()
{
	InitializeLevel();
	wchar_t filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename)); ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn); ofn.hwndOwner = NULL; ofn.lpstrFilter = L"Level file.\0*.txt\0";
	ofn.lpstrFile = filename; ofn.nMaxFile = MAX_PATH; ofn.lpstrTitle = L"Load a level .txt file.";
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&ofn)) {
		cout << red << "[SE] Failed to load level" << white << endl;
		current_file = "";
	}
	else {
		std::wstring ws(filename);
		std::string fl(ws.begin(), ws.end());

		current_file = fl;



		string d = fl.substr(fl.length() - 17, 2);
		if (d.substr(0, 1) == "\\")
		{
			d = d.substr(1, 1);
		}
		clevel = d;
		cout << green << "[SE] Loaded level " << d << " from " << current_file << white << endl;

		LoadLevelFromFile(fl);
	}
}