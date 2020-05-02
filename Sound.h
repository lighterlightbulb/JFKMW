#pragma once

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *sfxPorts[2];
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

	CurrentPacket >> music_data_size;
	delete[] music_data;
	music_data = new char[music_data_size];
	for (int i = 0; i < music_data_size; i++)
	{
		uint_fast8_t g;
		CurrentPacket >> g;
		music_data[i] = (char)g;
	}
	cout << green << "[Network] Received new music. " << music_data_size / 1024 << "kb big." << endl;

	doing_read = false;
	need_sync_music = true;
}
#endif

void SoundLoop()
{
	if (!networking || isClient)
	{
		for (uint_fast8_t i = 0; i < 2; i++)
		{
			uint_fast16_t RAM_P = i == 0 ? 0x1DF9 : 0x1DFC; //1DF9 and 1DFC

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

				string file = path + "Sounds/music/" + int_to_hex(old_1dfb, true) + ".ogg";
				music = Mix_LoadMUS(file.c_str());

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
					cout << purple << "[Audio] Playing " << file << white << endl;
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
				Mix_Music* music = Mix_LoadMUSType_RW(rw, MUS_OGG, 0);
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
			string file = path + "Sounds/music/" + int_to_hex(old_1dfb, true) + ".ogg";
			std::ifstream input(file, std::ios::in | std::ios::binary | std::ios::ate);
			if (input.is_open())
			{
				delete[] music_data;
				music_data_size = int(input.tellg());
				music_data = new char[music_data_size];
				input.seekg(0, std::ios::beg);
				input.read(music_data, music_data_size);
				input.close();
				cout << purple << "[Audio] Loaded " << file << white << endl;
				need_sync_music = true;
			}
			else
			{
				cout << purple << "[Audio] Failed to load " << file << white << endl;
			}
		}
	}
}