#pragma once

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *sfxPorts[3];
uint_fast16_t sound_table[3] = { 0x1DF9, 0x1DFC, 0x1DFA };
uint8_t old_1dfb = 0xFF;

bool init_audio()
{
	//Initialize SDL_mixer
	if (Mix_OpenAudio(ogg_sample_rate, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		cout << purple << "[Audio] Error : " << Mix_GetError() << white << endl;
		return false;
	}

	cout << purple << "[Audio] Initialized audio port (" << dec << ogg_sample_rate << "hz)." << white << endl;
	return true;
}

bool spc_or_ogg = false; //false = SPC, true = OGG
char *music_data;
int music_data_size;
#if not defined(DISABLE_NETWORK)
void SendMusic()
{
	while (doing_write) {
		sf::sleep(sf::milliseconds(1));
	}
	doing_read = true;
	cout << green << "[Network] Packing music data.." << endl;
	CurrentPacket << spc_or_ogg;
	CurrentPacket << music_data_size;
	for (int i = 0; i < music_data_size; i++)
	{
		CurrentPacket << (uint_fast8_t)music_data[i];
	}
	doing_read = false;
}

void ReceiveMusic(bool dont_care = false)
{
	if (!dont_care)
	{
		while (doing_write) {
			sf::sleep(sf::milliseconds(1));
		}
		kill_music = true;
		while (kill_music) {
			Sleep(1);
		}
	}
	doing_read = true;
	cout << green << "[Network] Downloading music from server.." << endl;

	CurrentPacket >> spc_or_ogg;
	CurrentPacket >> music_data_size;
	delete[] music_data;
	music_data = new char[music_data_size];
	for (int i = 0; i < music_data_size; i++)
	{
		uint_fast8_t g;
		CurrentPacket >> g;
		music_data[i] = (char)g;
	}
	cout << green << "[Network] Received new music. " << dec << music_data_size / 1024 << "kb big." << endl;

	doing_read = false;
	need_sync_music = true;
}
#endif

void SoundLoop()
{
	if (!networking || isClient)
	{
		for (uint_fast8_t i = 0; i < 3; i++)
		{
			uint_fast16_t RAM_P = sound_table[i];

			if (ASM.Get_Ram(RAM_P, 1) != 0)
			{
				Mix_FreeChunk(sfxPorts[i]);

				string sfx_to_play = path + "Sounds/" + int_to_hex(RAM_P) + "/" + int_to_hex(ASM.Get_Ram(RAM_P, 1), true) + ".ogg";
				const char* cstr = sfx_to_play.c_str();

				//cout << purple << "[Audio] Port" << dec << (i+1) << " : " << cstr << white << endl;


				sfxPorts[i] = Mix_LoadWAV(cstr);

				if (sfxPorts[i] == NULL) {
					cout << purple << "[Audio] Port " << dec << (i + 1) << " Error : " << Mix_GetError() << white << endl;
				}
				else {
					if (Mix_PlayChannel(-1, sfxPorts[i], 0) == -1) {
						cout << purple << "[Audio] Port " << dec << (i + 1) << " Error : " << Mix_GetError() << white << endl;
					}
				}
				ASM.Write_To_Ram(RAM_P, 0, 1);
			}

		}
		if (!networking)
		{
			if (ASM.Get_Ram(0x1DFB, 1) != old_1dfb)
			{
				old_1dfb = ASM.Get_Ram(0x1DFB, 1);

				string file2 = path + "Sounds/music/" + int_to_hex(old_1dfb, true) + ".ogg";
#if defined(USE_SDLMIXER_X)
				string file1 = path + "Sounds/music/" + int_to_hex(old_1dfb, true) + ".spc";
				if (is_file_exist(file1.c_str())) {
					music = Mix_LoadMUS(file1.c_str());
					spc_or_ogg = false;
				}
				else {
					music = Mix_LoadMUS(file2.c_str());
					spc_or_ogg = true;
				}
#else
				music = Mix_LoadMUS(file2.c_str());
				spc_or_ogg = true;
#endif

				if (music == NULL)
				{
					Mix_HaltMusic();
					cout << purple << "[Audio] Failed to change music : " << Mix_GetError() << white << endl;
				}
				else
				{
					if (Mix_PlayingMusic() == 1)
					{
						Mix_HaltMusic();
					}
					Mix_PlayMusic(music, -1);
					cout << purple << "[Audio] Playing song 0x" << hex << int_to_hex(old_1dfb, true) << dec << white << endl;
				}
			}
		}
		else
		{
			if (kill_music)
			{
				if (Mix_PlayingMusic() == 1)
				{
					Mix_HaltMusic();
				}
				kill_music = false;
			}
			if (need_sync_music)
			{
				need_sync_music = false;
				SDL_RWops* rw = SDL_RWFromMem(music_data, music_data_size);
#if defined(USE_SDLMIXER_X)
				Mix_Music* music = Mix_LoadMUSType_RW(rw, spc_or_ogg ? MUS_OGG : MUS_GME, 0);
#else
				//Linux users wont get sdl mixer x.
				Mix_Music* music;
				if (spc_or_ogg)
				{
					music = Mix_LoadMUSType_RW(rw, MUS_OGG, 0);
				}
				
#endif
				if (music == NULL)
				{
					Mix_HaltMusic();
					cout << purple << "[Audio] Failed to change music : " << Mix_GetError() << white << endl;
				}
				else
				{
					if (Mix_PlayingMusic() == 1)
					{
						Mix_HaltMusic();
					}
					Mix_PlayMusic(music, -1);
					cout << purple << "[Audio] Playing music" << white << endl;
				}
			}
		}
	}
	else
	{
		//Music additions
		if (ASM.Get_Ram(0x1DFB, 1) != old_1dfb)
		{
			old_1dfb = ASM.Get_Ram(0x1DFB, 1);
			string file1 = path + "Sounds/music/" + int_to_hex(old_1dfb, true) + ".spc";
			string file2 = path + "Sounds/music/" + int_to_hex(old_1dfb, true) + ".ogg";
			string file_picked;
			if (is_file_exist(file1.c_str())) {
				file_picked = file1;
				spc_or_ogg = false;
			}
			else {
				file_picked = file2;
				spc_or_ogg = true;
			}

			ifstream input(file_picked, ios::in | ios::binary | ios::ate);
			if (input.is_open())
			{
				delete[] music_data;
				music_data_size = int(input.tellg());
				music_data = new char[music_data_size];
				input.seekg(0, ios::beg);
				input.read(music_data, music_data_size);
				input.close();
				cout << purple << "[Audio] Loaded " << file_picked << white << endl;
				need_sync_music = true;
			}
			else
			{
				cout << purple << "[Audio] Failed to load " << file_picked << white << endl;
			}
		}
	}
}