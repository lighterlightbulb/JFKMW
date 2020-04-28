#pragma once

void player_code()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError(); return;
	}
	screen(resolution_x, resolution_y, "JFK mario world - " + da_epical_function_lol("JFKMarioWorld.exe"));
	init_audio();
	init_input();



	while (!actuallyquitgame)
	{
		disconnected = false;
		PlayerAmount = 0; SelfPlayerNumber = 1; CheckForPlayers();
		quit = false;
		SDL_ShowCursor(SDL_ENABLE);

		/* Options Loop */
		string s_or_c;
		std::cout << yellow << "[JFKMW] Press Q if you want to play singleplayer, W if connecting to a server (client), To host a server, start the game with the argument -s. We do not have a UI right now, but we're working on it." << white << endl;
		while (doneOnlyLoop(true))
		{
			cls();
			zsnes_ui.process();
			zsnes_ui.draw_button(4, 2, 50, 12);
			zsnes_ui.finish_processing(ren);

			//we create a rectangle for knowing where it will be drawn, in this case it will cover the whole screen.
			SDL_Rect dest;
			dest.x = sp_offset_x; dest.y = sp_offset_y; dest.w = 256 * scale; dest.h = 224 * scale;

			//we copy it to the renderer, for your program, if you want to have a ingame thing (variable or w/e), you just simply don't do this and render what your game has instead.
			SDL_Rect DestR;

			DestR.x = sp_offset_x;
			DestR.y = sp_offset_y;
			DestR.w = 256 * scale;
			DestR.h = 224 * scale;

			//Copied from renderer.h
			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					RenderBackground(
						(-int(double(CameraX) * (double(ServerRAM.RAM[0x3F06]) / 16.0)) % 512) + x * 512,
						-256 + (int(double(CameraY) * (double(ServerRAM.RAM[0x3F07]) / 16.0)) % 512) + y * -512);
				}
			}
			//Copied from renderer.h
			SDL_RenderCopy(ren, screen_t_l1, nullptr, &DestR);
			SDL_RenderCopy(ren, screen_t_l2, nullptr, &DestR);


			SDL_RenderCopy(ren, zsnes_ui.texture, NULL, &dest);

			redraw();
			check_input();
			if (state[SDL_SCANCODE_Q]) {
				s_or_c = "t";
				break;
			}
			if (state[SDL_SCANCODE_W]) {
				s_or_c = "c";
				break;
			}

			if (state[SDL_SCANCODE_ESCAPE])
			{
				actuallyquitgame = true;
				break;
			}

		}

		if (actuallyquitgame)
		{
			break;
		}
		/* Load Shit */

		networking = s_or_c != "t";
		initialize_map16();

		if (s_or_c == "c")
		{
			isClient = true;
			std::cout << "Please input a IP." << std::endl; std::cin >> ip;
			std::cout << "Please input the port." << std::endl; std::cin >> PORT;
			if (ConnectClient() == false) {
				networking = false;
				isClient = false;
				s_or_c = "t";
			}
		}
		
		if (s_or_c == "t")
		{
			isClient = false;
			string level = ""; std::cout << "Enter a level : "; std::cin >> level; LevelManager.LoadLevel(stoi(level, nullptr, 16));
		}

		//Initialize Singleplayer
		if (s_or_c == "t") {
			PlayerAmount = 1; SelfPlayerNumber = 1; CheckForPlayers();
		}

		//Initialize Multiplayer Client
		if (networking)
		{
			thread = new sf::Thread(&NetWorkLoop); thread->launch();
		}

		std::cout << yellow << "[JFKMW] Waiting for player..." << white << endl;
		while (Mario.size() == 0) {
			Sleep(16);
		}

		if (!isClient) {
			game_init();
		}
		SDL_ShowCursor(SDL_DISABLE);


		while (!done(true))
		{
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			check_input(); game_loop(); SoundLoop();

			render();
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

			redraw();
			total_time_ticks = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

			add_to_ticks = 0;

			if (disconnected) {
				quit = true; cout << red << "[Network] Disconnected." << white << endl; break;
			}
		}

		//We quit the game go back to the ZSNES ui
		if (networking && !disconnected) { socketG.disconnect();  thread->terminate(); }
		cout << green << "Returning to main screen..." << white << endl;
		Sleep(1000);
		quit = true;
	}
}