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

		chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
		game_loop();
		SoundLoop();
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		total_time_ticks = chrono::duration_cast<chrono::duration<double>>(t2 - t1);

		

		if (!(global_frame_counter % 120) && clients.size() > 0)
		{
			if (DisablePrints)
			{
				cout << green << "[Network] latest loop : "
					<< dec << total_time_ticks.count() * 1000.0 << " ms. "
					<< (double(data_size_current) / 2048.0) << "kb/s" << white << endl;
			}
			data_size_current = 0;
		}

		if (getKey(VK_F1))
		{
			string level = ""; cout << "Enter a level : "; cin >> level; LevelManager.LoadLevel(stoi(level, nullptr, 16));
		}
		if (getKey(VK_F2))
		{
			cout << green << "[Network] Syncing RAM to other players.." << endl;
			Set_Server_RAM();
			recent_big_change = true;
		}
		if (getKey(VK_F3))
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
		Sleep(16);
	}
}