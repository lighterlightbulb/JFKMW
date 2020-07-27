#pragma once

void server_code(string level = "")
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS);

	game_init();
	initialize_map16();
	cout << yellow << "[JFKMW] Starting up a server." << white << endl;
	if (level == "") {
		level = "1";
		//cout << "Enter a level : "; cin >> level;
	}
	LevelManager.LoadLevel(stoi(level, nullptr, 16));

	isClient = false;
	networking = true;
	bool DisablePrints = false;
	
	cout << green << "[Network] Server starting. Commands :" << endl;
	cout << "F1 = Load a new level" << endl;
	cout << "F2 = Sync RAM to all players" << endl;
	cout << "F3 = Status Prints Toggle" << endl;
	cout << "F4 = Sync Music" << endl;
	cout << "F5 = Change RAM Variable" << endl;
	cout << "F6 = PVP Toggle" << endl;
	cout << "F7 = Dump RAM" << endl;
	cout << "F8 = Dump Level data" << endl;
	cout << "F9 = Reload Lua" << white << endl;

	data_size_current = 0;
	thread = new sf::Thread(&NetWorkLoop); thread->launch();

	int FPS = 60;
	while (true)
	{

		while (doing_read) {
			SDL_Delay(1);
		}

		doing_write = true;
		Uint32 start_time = SDL_GetTicks();

		
		chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
		game_loop();
		SoundLoop();
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		total_time_ticks = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
		

		if (!(global_frame_counter % 60) && clients.size() > 0)
		{
			if (DisablePrints)
			{
				cout << green << "[Network] latest loop : "
					<< dec << total_time_ticks.count() * 1000.0 << " ms. "
					<< (double(data_size_current) / 1024.0) << "kb/s in, " << (double(data_size_now) / 1024.0) << "kb/s out" << white << endl;
			}
			data_size_current = 0;
			data_size_now = 0;
		}

		if (getKey(0x70))
		{
			
			string level = ""; cout << "Enter a level : "; cin >> level; 
			doing_write = true;


			ASM.Write_To_Ram(0x3f08, stoi(level, nullptr, 16), 2);
			ASM.Write_To_Ram(0x1493, 0x06, 1);

			doing_write = false;

			discord_message("Switched to level " + level);
		}
		if (getKey(0x71))
		{
			cout << green << "[Network] Syncing RAM to other players.." << endl;
			Set_Server_RAM();
			recent_big_change = true;
			
		}
		if (getKey(0x72))
		{
			DisablePrints = !DisablePrints;
			if (!DisablePrints)
			{
				cout << green << "[Network] Disabled Network latest loop prints." << endl;
			}
			else
			{
				cout << green << "[Network] Enabled Network latest loop prints." << endl;
			}
		}

		if (getKey(0x73))
		{
			cout << green << "[Network] Syncing music to other players.." << endl;
			need_sync_music = true;
		}
		if (getKey(0x74))
		{

			string r = ""; cout << "RAM $ to modify : "; cin >> r;
			string v = ""; cout << "New Value (hex) : "; cin >> v;
			RAM[stoi(r, nullptr, 16)] = uint_fast8_t(stoi(v, nullptr, 16));
		}
		if (getKey(0x75))
		{
			pvp = !pvp;
			if (!pvp)
			{
				cout << green << "[Network] Disabled PVP." << endl;
			}
			else
			{
				cout << green << "[Network] Enabled PVP." << endl;
			}

			Time_ChatString = 300;
			Curr_ChatString = pvp ? "pvp is now enabled" : "pvp is now off";

		}
		if (getKey(0x76))
		{
			dump_ram();
		}
		if (getKey(0x77))
		{
			dump_level_data();
		}
		if (getKey(0x78))
		{
			doing_write = true;
			lua_loadfile(last_lua_file);
			doing_write = false;
		}
		doing_write = false;

		if (Uint32(1000 / FPS) > (SDL_GetTicks() - start_time))
		{
			SDL_Delay((1000 / FPS) - (SDL_GetTicks() - start_time));
		}
	}
}