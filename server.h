#pragma once

void server_code(string level = "")
{
	game_init();
	initialize_map16();
	cout << yellow << "[JFKMW] Starting up a server." << white << endl;
	if (level == "") {
		cout << "Enter a level : "; cin >> level;
	}
	LevelManager.LoadLevel(stoi(level, nullptr, 16));

	isClient = false;
	networking = true;
	bool DisablePrints = true;
	
	cout << green << "[Network] Server starting. To load a new level, press F1, to sync the ram to all players, press F2." << white << endl;
	data_size_current = 0;
	thread = new sf::Thread(&NetWorkLoop); thread->launch();
	Sleep(100);
	while (true)
	{
		while (doing_read) {
			Sleep(1);
		}


		doing_write = true;
		chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
		game_loop();
		SoundLoop();
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		total_time_ticks = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
		doing_write = false;
		

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
			LevelManager.LoadLevel(stoi(level, nullptr, 16));
			game_init();
			Set_Server_RAM();
			recent_big_change = true;
			doing_write = false;
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
		Sleep(16);
	}
}