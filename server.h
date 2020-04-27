#pragma once

void server_code(string level = "")
{
	game_init();
	initialize_map16();
	std::cout << yellow << "[JFKMW] Starting up a server." << white << endl;
	if (level == "") {
		std::cout << "Enter a level : "; std::cin >> level;
	}
	LevelManager.LoadLevel(stoi(level, nullptr, 16));

	isClient = false;
	networking = true;
	
	cout << green << "[Network] Server starting. To load a new level, press F1, to sync the ram to all players, press F2." << white << endl;
	data_size_current = 0;
	thread = new sf::Thread(&NetWorkLoop); thread->launch();
	while (true)
	{

		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		game_loop();
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		total_time_ticks = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		Sleep(16);
		if (!(global_frame_counter % 60) && clients.size() > 0)
		{
			cout << green << "[Network] latest loop : "
				<< dec << total_time_ticks.count() * 1000.0 << " ms. "
				<< (double(data_size_current) / 1024.0) << "kb data outbound." << white << endl;
			data_size_current = 0;
		}

		if (getKey(VK_F1))
		{
			string level = ""; std::cout << "Enter a level : "; std::cin >> level; LevelManager.LoadLevel(stoi(level, nullptr, 16));
		}
		if (getKey(VK_F2))
		{
			cout << green << "[Network] Syncing RAM to other players.." << endl;
			recent_big_change = true;
			Set_Server_RAM();
		}
	}
}