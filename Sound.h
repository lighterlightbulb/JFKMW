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

std::vector<char> data;

void SoundLoop()
{

	for (uint_fast8_t i = 0; i < 2; i++)
	{
		uint_fast16_t RAM_P = i == 0 ? 0x1DF9 : 0x1DFC; //1DF9 and 1DFC
	
		if (ASM.Get_Ram(RAM_P, 1) != 0)
		{
			Mix_FreeChunk(sfxPorts[i]);

			string sfx_to_play = path + "Sounds/" + int_to_hex(RAM_P) + "/" + int_to_hex(ASM.Get_Ram(RAM_P,1), true) + ".ogg"; 
			const char *cstr = sfx_to_play.c_str();

			cout << purple << "[Audio] Port" << dec << (i+1) << " : " << cstr << white << endl;


			sfxPorts[i] = Mix_LoadWAV(cstr);

			if (sfxPorts[i] == NULL) {
				cout << purple << "[Audio] Error : " << Mix_GetError() << white << endl;
			}
			else {
				if (Mix_PlayChannel(-1, sfxPorts[i], 0) == -1) { 
					cout << purple << "[Audio] Error : " << Mix_GetError() << white << endl; 
				}
			}
			ASM.Write_To_Ram(RAM_P, 0, 1);
		}

	}

	if (ASM.Get_Ram(0x1DFB, 1) != old_1dfb)
	{
		old_1dfb = ASM.Get_Ram(0x1DFB, 1);

		string file = path + "Sounds/music/" + std::to_string(old_1dfb) + ".ogg";
		music = Mix_LoadMUS(file.c_str());

		if (music == NULL)
		{
			cout << purple << "[Audio] Failed to change music : " << Mix_GetError() << white << endl;
		}
		else
		{
			if (Mix_PlayingMusic() == 1)
			{
				Mix_HaltMusic();
				add_to_ticks += 30;
				Sleep(simulate_SNES_quirks * 500);
			}
			Mix_PlayMusic(music, -1);
			cout << purple << "[Audio] Playing " << file << white << endl;
		}
	}

}