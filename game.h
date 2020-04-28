#pragma once


void game_init()
{
	decode_graphics_file("Graphics/exanimations.bin", 8);
	decode_graphics_file("Graphics/hud.bin", 12);
}

void game_loop()
{
	while (doing_read) {}

	doing_write = true;
	global_frame_counter += 1;
	if (networking)
	{
		if ((clients.size() == 0 && !isClient))
		{
			return;
		}
	}

	mapWidth = ServerRAM.RAM[0x3F00] + ServerRAM.RAM[0x3F01] * 256;
	if (mapWidth == 0)
	{
		mapWidth = 256;
	}
	mapHeight = ServerRAM.RAM[0x3F02] + ServerRAM.RAM[0x3F03] * 256;

	CheckForPlayers();

	process_ex_animation();

	if (!isClient || !networking) //if we are the server or we are playing locally...
	{
		for (int i = 0; i < 0x200; i++) //Clear OAM loop
		{
			ServerRAM.RAM[0x200 + i] = 0;
		}
		Sprites.process_all_sprites(); //we're processing sprites. we're either the server or a player in local mode.
	}

	uint_fast8_t player = 1;
	unsigned int camera_total_x = 0; unsigned int camera_total_y = 0;
	for (std::list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrPlayer = *item;
		CurrPlayer.PlayerControlled = networking ? (player == SelfPlayerNumber) : true; //Only control myself
		if (!isClient && networking)
		{
			CurrPlayer.PlayerControlled = false;
		}
		if (player == SelfPlayerNumber)
		{
			CurrPlayer.skin = my_skin;
		}
		//cout << SelfPlayerNumber;
		CurrPlayer.player_index = player;
		CurrPlayer.Process();

		camera_total_x += unsigned int((CurrPlayer.CAMERA_X - 128.0) > 0 ? (CurrPlayer.CAMERA_X - 128.0) : 0);
		camera_total_y += unsigned int((CurrPlayer.CAMERA_Y - 112.0) > 0 ? (CurrPlayer.CAMERA_Y - 112.0) : 0);

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
			ASM.start_JFK_thread(location_rom_levelasm); //This runs the ASM.
		}
		if (lua_loaded) {
			lua_run_main();
		}

		ServerRAM.RAM[0x14] = global_frame_counter % 256;
	}

	if (networking && !isClient) //if we are the server
	{
		memcpy(&ServerRAM_D.RAM, &ServerRAM.RAM, RAM_Size * sizeof(uint_fast8_t));
		doing_write = false;
	}

}