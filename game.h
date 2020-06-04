#pragma once


void game_init()
{
	decode_graphics_file("Graphics/exanimations.bin", 8);
	decode_graphics_file("Graphics/hud.bin", 11);
	for (uint_fast16_t l = 0; l < 0x800; l += 2)
	{
		ServerRAM.RAM[VRAM_Location + 0xB800 + l] = 0x7F;
		ServerRAM.RAM[VRAM_Location + 0xB800 + l + 1] = 0x00;

	}
}

void game_loop()
{
	global_frame_counter += 1;
#if not defined(DISABLE_NETWORK)
	if (networking)
	{

		if ((clients.size() == 0 && !isClient))
		{
			return;
		}
	}
#endif
	if (!isClient && ServerRAM.RAM[0x1493] > 0)
	{
		if ((global_frame_counter % 3) == 0)
		{
			ServerRAM.RAM[0x1493] -= 1;
		}
		if (ServerRAM.RAM[0x1493] == 0)
		{
			if (ASM.Get_Ram(0x3F0A, 2) != 0)
			{
				LevelManager.LoadLevel(uint_fast16_t(ASM.Get_Ram(0x3F0A, 2)));

				game_init();

				ASM.Write_To_Ram(0x3F0A, 0, 2);
			}
			else
			{
				ASM.Write_To_Ram(0x1DFB, 0, 1);
			}
		}
	}

	if (!isClient && ServerRAM.RAM[0x1887] > 0)
	{
		ServerRAM.RAM[0x1887]--;
	}

	if (ServerRAM.RAM[0x1887] > 0)
	{
		if (!networking || isClient)
		{
			vibrate_controller(1.0, 32);
		}
	}

	mapWidth = ServerRAM.RAM[0x3F00] + ServerRAM.RAM[0x3F01] * 256;
	if (mapWidth == 0)
	{
		mapWidth = 256;
	}
	mapHeight = ServerRAM.RAM[0x3F02] + ServerRAM.RAM[0x3F03] * 256;

	LevelManager.start_x = ServerRAM.RAM[0x3F0B] + ServerRAM.RAM[0x3F0C] * 256;
	LevelManager.start_y = ServerRAM.RAM[0x3F0D] + ServerRAM.RAM[0x3F0E] * 256;

	CheckForPlayers();

	process_ex_animation();

	if (!isClient || !networking) //if we are the server or we are playing locally...
	{
		for (uint_fast16_t i = 0; i < 0x400; i++) //Clear OAM loop
		{
			ServerRAM.RAM[0x200 + i] = 0;
		}
		Sprites.process_all_sprites(); //we're processing sprites. we're either the server or a player in local mode.
	}

	uint_fast8_t player = 1;
	int camera_total_x = 0; int camera_total_y = 0;
	for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrPlayer = *item;
		//cout << "Player " << dec << int(player) << " location is " << hex << &CurrPlayer << dec << endl;
		CurrPlayer.PlayerControlled = networking ? (player == SelfPlayerNumber) : true; //Only control myself
		if (!isClient && networking) {
			CurrPlayer.PlayerControlled = false;
		}
		if (CurrPlayer.PlayerControlled == true){
			CurrPlayer.skin = my_skin;
			for (int i = 0; i < player_name_size; i++)
			{
				if (i >= username.length())
				{
					CurrPlayer.player_name_cut[i] = ' ';
				}
				else
				{
					CurrPlayer.player_name_cut[i] = username.at(i);
				}
			}
		}
		//cout << SelfPlayerNumber;
		CurrPlayer.player_index = player;
		CurrPlayer.Process();

		camera_total_x += int((CurrPlayer.CAMERA_X - 128.0) > 0 ? (CurrPlayer.CAMERA_X - 128.0) : 0);
		camera_total_y += int((CurrPlayer.CAMERA_Y - 112.0) > 0 ? (CurrPlayer.CAMERA_Y - 112.0) : 0);

		player += 1;


	}
	PlayerInteraction();

	camera_total_x /= uint_fast32_t(Mario.size());
	camera_total_y /= uint_fast32_t(Mario.size());

	if (!isClient)
	{
		//cout << "clearing OAM";

		if (ServerRAM.RAM[0x1411] != 0)
		{
			ASM.Write_To_Ram(0x1462, uint_fast32_t(camera_total_x), 2);
		}
		if (ServerRAM.RAM[0x1412] != 0)
		{
			ASM.Write_To_Ram(0x1464, uint_fast32_t(camera_total_y), 2);
		}
		if (asm_loaded) {
			ASM.start_JFK_thread(); //This runs the ASM.
		}
		if (lua_loaded) {
			lua_run_main();
		}

		ServerRAM.RAM[0x14] = global_frame_counter % 256;
	}

}