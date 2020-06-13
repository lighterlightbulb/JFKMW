#pragma once

void draw_number_hex(uint_fast8_t pos_x, uint_fast8_t pos_y, uint_fast16_t number, int length)
{
	for (int i = 0; i < length; i++)
	{
		VRAM[0xB800 + (-i * 2) + (pos_x * 2) + pos_y * 64] = (number >> (i << 2)) & 0xF;
		VRAM[0xB801 + (-i * 2) + (pos_x * 2) + pos_y * 64] = 6;
	}

}

void draw_number_dec(uint_fast8_t pos_x, uint_fast8_t pos_y, int number)
{
	int length = int(to_string(number).length());
	for (int i = 0; i < length; i++)
	{
		VRAM[0xB800 + (-i * 2) + (pos_x * 2) + pos_y * 64] = int(number / pow(10, i)) % 10;
		VRAM[0xB801 + (-i * 2) + (pos_x * 2) + pos_y * 64] = 6;
	}

}

void render_oam(uint_fast16_t offset_o = 0, int CameraX = 0, int CameraY = 0)
{
	for (uint_fast16_t i = 0; i < 0x400; i += 8) //Tile, Size, XY (4 bytes), PAL, ANG, in total 8 bytes per entry. 0 to 7.
	{

		uint_fast8_t size = ServerRAM.RAM[offset_o + i + 1];
		int_fast16_t size_x = (size & 0xF) << 4;
		int_fast16_t size_y = ((size >> 4) & 0xF) << 4;
		int x_position = ServerRAM.RAM[offset_o + i + 2] + int_fast8_t(ServerRAM.RAM[offset_o + i + 3]) * 256;
		int y_position = ServerRAM.RAM[offset_o + i + 4] + int_fast8_t(ServerRAM.RAM[offset_o + i + 5]) * 256;
		uint_fast8_t flags = ServerRAM.RAM[offset_o + i + 6] >> 4;
		uint_fast16_t tile = ServerRAM.RAM[offset_o + i] + ((flags & 1) << 8);

		if (tile != 0x0 &&
			(x_position - CameraX) > -size_x && (x_position - CameraX) < (256 + size_x) &&
			(y_position - CameraY) > (-16 + -size_y) && (y_position - CameraY) < (224 + size_y)			
		)
		{
			uint_fast8_t pal = ServerRAM.RAM[offset_o + i + 6] & 0xF;
			double angle = (double(ServerRAM.RAM[offset_o + i + 7]) / 256.0) * 360.0;
			draw_tile_custom(x_position - CameraX, 224 - 32 - y_position + CameraY, size, angle, tile, pal, 
				SDL_RendererFlip(
				((flags >> 1) & 1) +
				(((flags >> 2) & 1) << 1)
				)
			);
		}
	}
}

void render()
{
	CheckForPlayers();
	if (Mario.size() < 1 || SelfPlayerNumber < 1)
	{
		return;
	}

	if (ServerRAM.RAM[0x1493] > 0)
	{
		if (screen_darken < 255)
		{
			screen_darken += 1;
		}
	}
	else
	{
		screen_darken = 0;
	}

	for (uint_fast16_t i = 0; i < 256; i++)
	{
		palette_array[i] = ServerRAM.RAM[0x3D00 + i] + (ServerRAM.RAM[0x3E00 + i] << 8);
	}
	memcpy(VRAM, &ServerRAM.RAM[VRAM_Location], VRAM_Size * sizeof(uint_fast8_t));



	MPlayer& LocalPlayer = get_mario(SelfPlayerNumber);
	CameraX = int_fast16_t(LocalPlayer.CAMERA_X - 120.0);
	CameraY = int_fast16_t(max(0.0, LocalPlayer.CAMERA_Y - 112.0));
	if (ServerRAM.RAM[0x1887] > 0)
	{
		CameraY += (global_frame_counter % 3);
	}

	if (CameraX < 0) { CameraX = 0; }
	if (CameraY < 0) { CameraY = 0; }
	if (CameraX > (-256 + int_fast16_t(mapWidth) * 16))
	{
		CameraX = (-256 + int_fast16_t(mapWidth) * 16);
	}
	if (CameraY > (-224 + int_fast16_t(mapHeight) * 16))
	{
		CameraY = (-224 + int_fast16_t(mapHeight) * 16);
	}

	int_fast16_t offsetX = int_fast16_t(CameraX / 16.0);
	uint_fast8_t offsetXPixel = uint_fast8_t(CameraX) % 16;
	int_fast16_t offsetY = int_fast16_t(CameraY / 16.0);
	uint_fast8_t offsetYPixel = uint_fast8_t(CameraY) % 16;

	//Draw BG

	if (ServerRAM.RAM[0x3F05] != curr_bg)
	{
		curr_bg = ServerRAM.RAM[0x3F05];
		SDL_DestroyTexture(bg_texture);
		SDL_FreeSurface(bg_surface);
		bg_surface = loadSurface(path + "Sprites/backgrounds/Background" + to_string(int(curr_bg)) + ".png");
		bg_texture = SDL_CreateTextureFromSurface(ren, bg_surface);

	}

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			RenderBackground(
				(-int(double(CameraX) * (double(ServerRAM.RAM[0x3F06]) / 16.0) + ASM.Get_Ram(0x1466, 2)) % 512) + x*512,
				-256 + (int(double(CameraY) * (double(ServerRAM.RAM[0x3F07]) / 16.0) + ASM.Get_Ram(0x1468, 2)) % 512) + y*-512);
		}
	}

	// Start rendering, by locking surface.
	SDL_LockSurface(&screen_s_l1);
	SDL_Surface *screen_plane = &screen_s_l1;
	SDL_memset(screen_plane->pixels, 0, screen_plane->h * screen_plane->pitch);


	//Draw scenery
	uint_fast8_t blocks_on_screen = 0;
	
	for (uint_fast8_t x = 0; x < 17; x++)
	{
		for (uint_fast8_t y = 0; y < 15; y++)
		{
			uint_fast16_t tile = map16_handler.get_tile(x + offsetX, y + offsetY);
			if (tile != 0x25)
			{
				blocks_on_screen += 1;
				uint_fast16_t entry = tile * tile_table_size;

				//Block 8x8 tiles
				for (uint_fast8_t i = 0; i < 4; i++)
				{
					uint_fast16_t block_index = map16_entries[entry + 1 + (i << 1)] + (map16_entries[entry + (i << 1)] << 8);
					uint_fast8_t index = map16_entries[(i <= 1 ? entry + tile_palette_1 : entry + tile_palette_2)];
					uint_fast8_t block_palette = (i & 1) ? (index & 0xF) : (index >> 4);

					if (block_index != 0xF8)
					{
						draw8x8_tile(
							((i << 3) & 0xF) - offsetXPixel + (x << 4),
							208 - (i > 1 ? -8 : 0) + offsetYPixel - (y << 4),
							block_index, block_palette
						);
					}

				}
			}
		}
	}


	//End rendering
	SDL_UnlockSurface(&screen_s_l1);
	//We can now draw the screen finished product.

	SDL_Rect DestR;

	DestR.x = sp_offset_x;
	DestR.y = sp_offset_y;
	DestR.w = 256 * scale;
	DestR.h = 224 * scale;

	SDL_DestroyTexture(screen_t_l1);
	screen_t_l1 = SDL_CreateTextureFromSurface(ren, &screen_s_l1);
	SDL_RenderCopy(ren, screen_t_l1, nullptr, &DestR);

	//Draw screen darkening
	SDL_SetRenderDrawColor(ren, 0, 0, 0, (screen_darken >> 3) << 3);
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(ren, NULL);
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);


	//Draw OAM (priority)
	render_oam(0x200, int(CameraX), int(CameraY));

	//Draw Mario
	for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrentMario = *item;

		if (CurrentMario.x > (CameraX - camBoundX) && CurrentMario.y > (CameraY - camBoundY) && CurrentMario.x < (CameraX + 256 + camBoundX) && CurrentMario.y < (CameraY + 224 + camBoundY))
		{
			float is_skidding = 1.f - (float(abs(CurrentMario.SKIDDING)) * 2.f);

			int offs = 0;

			if (!CurrentMario.invisible)
			{
				Sprite Mario(path + "Sprites/mario/" + to_string(CurrentMario.skin) + "/" + CurrentMario.sprite + ".png", -8 + int(CurrentMario.x) - int(CameraX), 224 - 32 - int(CurrentMario.y) + int(CameraY), int(CurrentMario.to_scale * is_skidding) * 32, 32);
			}

			if (CurrentMario.GRABBED_SPRITE != 0xFF && !CurrentMario.in_pipe)
			{
				uint_fast8_t tile = uint_fast8_t(ASM.Get_Ram(0x2F00 + CurrentMario.GRABBED_SPRITE, 1));
				uint_fast8_t size = uint_fast8_t(((ASM.Get_Ram(0x2E80 + CurrentMario.GRABBED_SPRITE, 1) & 0x7F) >> 4) + (((ASM.Get_Ram(0x2E80 + CurrentMario.GRABBED_SPRITE, 1) & 0x7F) >> 4) << 4));

				int_fast16_t x_position = int_fast16_t(double(CurrentMario.x + CurrentMario.to_scale * -12.0));
				int_fast16_t y_position = int_fast16_t(double(CurrentMario.y - (CurrentMario.STATE > 0 ? 13.0 : 16.0)));

				uint_fast8_t pal = uint_fast8_t(ASM.Get_Ram(0x2E80 + CurrentMario.GRABBED_SPRITE, 1) & 0xF);

				double angle = 0.0;
				if (tile != 0x0 &&
					(x_position - CameraX) > -64 && (x_position - CameraX) < (256 + 64) &&
					(y_position - CameraY) > -64 && (y_position - CameraY) < (224 + 64)
					)
				{
					draw_tile_custom(x_position - CameraX, 224 - 32 - y_position + CameraY, size, angle, tile, pal, SDL_FLIP_NONE);
				}
			}
		}
	}

	//Start draw to layer 3
	SDL_LockSurface(&screen_s_l2);
	SDL_Surface* screen_plane_sequel = &screen_s_l2;
	SDL_memset(screen_plane_sequel->pixels, 0, screen_plane_sequel->h* screen_plane_sequel->pitch);


	//Status bar code here
	for (int i = 0; i < 5; i++)
	{

		uint_fast8_t new_l = uint_fast8_t(LocalPlayer.player_name_cut[i]);
		if (new_l == 0x20) { new_l = 0x57 + 0x7F; }
		if (new_l < 0x3A) { new_l = new_l - 0x30 + 0x57; }

		VRAM[0xB804 + (i * 2) + 128] = new_l - 0x57;
		VRAM[0xB805 + (i * 2) + 128] = 2;
	}

	//WO's
	VRAM[0xB806 + 192] = 0x26;
	VRAM[0xB807 + 192] = 0x6;
	draw_number_dec(5, 3, LocalPlayer.WO_counter);

	//Player X/Y
	draw_number_hex(21, 2, int(LocalPlayer.x), 4);
	draw_number_hex(26, 2, int(LocalPlayer.y), 4);

	//Player Speed X/Y
	draw_number_hex(16, 2, uint_fast16_t(LocalPlayer.Y_SPEED * 256.0), 4);
	draw_number_hex(11, 2, uint_fast16_t(LocalPlayer.X_SPEED * 256.0), 4);

	//Networking symbols
	VRAM[0xB800 + 20 + 192] = networking ? 0x17 : 0x15; VRAM[0xB801 + 20 + 192] = 6;
	VRAM[0xB800 + 18 + 192] = 0x3A; VRAM[0xB801 + 18 + 192] = 6;
	VRAM[0xB800 + 16 + 192] = isClient ? 0xC : 0x1C; VRAM[0xB801 + 16 + 192] = 6;

	//FCounter
	draw_number_hex(29, 2, ServerRAM.RAM[0x14], 2);

	//Ping
	VRAM[0xB800 + 56 + 192] = 0x16;	VRAM[0xB801 + 56 + 192] = 6;
	VRAM[0xB800 + 58 + 192] = 0x1C;	VRAM[0xB801 + 58 + 192] = 6;
	draw_number_dec(27, 3, ((abs(latest_server_response) % 3600) % 1000)/3);

	//FPS
	VRAM[0xB800 + 44 + 192] = 0xF;	VRAM[0xB801 + 44 + 192] = 6;
	VRAM[0xB800 + 46 + 192] = 0x19;	VRAM[0xB801 + 46 + 192] = 6;
	VRAM[0xB800 + 48 + 192] = 0x1C;	VRAM[0xB801 + 48 + 192] = 6;
	draw_number_dec(21, 3, int(1.0 / (total_time_ticks.count() / 1.0)));

	//KB
	//VRAM[0xB800 + 26 + 192] = 0x24;	VRAM[0xB801 + 26 + 192] = 6;
	VRAM[0xB800 + 30 + 192] = 0x14;	VRAM[0xB801 + 30 + 192] = 6;
	VRAM[0xB800 + 32 + 192] = 0x0B;	VRAM[0xB801 + 32 + 192] = 6;
	if (!(global_frame_counter % 60))
	{
		data_size_now = data_size_current;
		data_size_current = 0;
	}
	draw_number_dec(14, 3, data_size_now / 1024);

	//Render chat
	if (Time_ChatString > 0 || Chatting)
	{
		for (int i = 0; i < 32; i++)
		{
			for (int e = 16; e < 28; e++)
			{
				VRAM[0xB800 + (i << 1) + (e << 6)] = 0x7F;
				VRAM[0xB801 + (i << 1) + (e << 6)] = 6;
			}
		}


		//Congrats this will do it

		string C_String = Chatting ? (Typing_In_Chat + ((global_frame_counter % 20) > 10 ? "." : "")) : Curr_ChatString;
		int C_len = int(Chatting ? Typing_In_Chat.length() : Curr_ChatString.length());

		for (int i = 0; i < C_String.length(); i++)
		{
			int y = (C_len > 31) ? 25 : 26;
			uint_fast8_t new_l = uint_fast8_t(C_String.at(i));
			if (new_l == 0x20) { new_l = 0x57 + 0x7F; }
			if (C_String.at(i) == '<') { new_l = 0x2C + 0x57; }
			if (C_String.at(i) == '>') { new_l = 0x2D + 0x57; }
			if (new_l < 0x3A) { new_l = new_l - 0x30 + 0x57; }

			VRAM[0xB802 + (i * 2) + (y << 6)] = new_l - 0x57;
			VRAM[0xB803 + (i * 2) + (y << 6)] = 6;

		}

	}

	//Player list logic (shouldn't be here, but oh well)
	bool stat = (state[input_settings[8]]) || BUTTONS_GAMEPAD[9];
	if (stat != pressed_select)
	{
		pressed_select = stat;
		if (stat)
		{
			ServerRAM.RAM[0x1DFC] = 0x15;
			showing_player_list = !showing_player_list;
		}
	}

	//Player list
	if (showing_player_list)
	{
		for (int i = 0; i < 32; i++)
		{
			for (int e = 16; e < 28; e++)
			{
				VRAM[0xB800 + (i << 1) + (e << 6)] = 0x7F;
				VRAM[0xB801 + (i << 1) + (e << 6)] = 6;
			}
		}


		int y = 26;
		int plr_numb = 1;
		for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
		{
			MPlayer& CurrentMario = *item;
			//Draw the PlayerNumber) icon
			VRAM[0xB802 + (y << 6)] = plr_numb;
			VRAM[0xB803 + (y << 6)] = 6;
			VRAM[0xB804 + (y << 6)] = 0x3C;
			VRAM[0xB805 + (y << 6)] = 6;
			for (int i = 0; i < player_name_size; i++)
			{
				uint_fast8_t new_l = uint_fast8_t(CurrentMario.player_name_cut[i]);
				if (new_l == 0x20) { new_l = 0x57 + 0x7F; }
				if (new_l < 0x3A) { new_l = new_l - 0x30 + 0x57; }

				VRAM[0xB808 + (i * 2) + (y << 6)] = new_l - 0x57;
				VRAM[0xB809 + (i * 2) + (y << 6)] = 6;
			}

			//KO's
			draw_number_dec(17, y, CurrentMario.KO_counter);
			VRAM[0xB826 + (y << 6)] = 0x14; VRAM[0xB827 + (y << 6)] = 6;
			VRAM[0xB828 + (y << 6)] = 0x18; VRAM[0xB829 + (y << 6)] = 6;
			VRAM[0xB82A + (y << 6)] = 0x38; VRAM[0xB82B + (y << 6)] = 6;
			VRAM[0xB82C + (y << 6)] = 0x1C; VRAM[0xB82D + (y << 6)] = 6;

			//WO's
			draw_number_dec(25, y, CurrentMario.WO_counter);
			VRAM[0xB836 + (y << 6)] = 0x20; VRAM[0xB837 + (y << 6)] = 6;
			VRAM[0xB838 + (y << 6)] = 0x18; VRAM[0xB839 + (y << 6)] = 6;
			VRAM[0xB83A + (y << 6)] = 0x38; VRAM[0xB83B + (y << 6)] = 6;
			VRAM[0xB83C + (y << 6)] = 0x1C; VRAM[0xB83D + (y << 6)] = 6;




			y -= 1;
			plr_numb += 1;
		}
	}

	for (list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrentMario = *item;

		if (!CurrentMario.PlayerControlled && CurrentMario.x > (CameraX + 16) && CurrentMario.y > (CameraY) && CurrentMario.x < (CameraX + 224) && CurrentMario.y < (CameraY + 160))
		{
			for (int i = 0; i < 5; i++)
			{
				uint_fast8_t new_l = uint_fast8_t(CurrentMario.player_name_cut[i]);
				if (new_l == 0x20) { new_l = 0x57 + 0x7F; }
				if (new_l < 0x3A) { new_l = new_l - 0x30 + 0x57; }

				draw8x8_tile_2bpp(-12 + int(CurrentMario.x) - int(CameraX) + i * 8, 224 - int(CurrentMario.y + (CurrentMario.STATE ? 40 : 32)) + int(CameraY), new_l - 0x57, 6);
			}
		}
	}


	//Draw L3
	for (uint_fast8_t t3_x = 0; t3_x < 32; t3_x++)
	{
		for (uint_fast8_t t3_y = 0; t3_y < 28; t3_y++)
		{
			if (VRAM[0xB800 + (t3_x * 2) + t3_y * 64] != 0xFC)
			{
				draw8x8_tile_2bpp(t3_x * 8, t3_y * 8, VRAM[0xB800 + (t3_x * 2) + t3_y * 64], VRAM[0xB801 + (t3_x * 2) + t3_y * 64]);
			}
		}
	}

	SDL_UnlockSurface(&screen_s_l2);

	SDL_DestroyTexture(screen_t_l2);
	screen_t_l2 = SDL_CreateTextureFromSurface(ren, &screen_s_l2);
	SDL_RenderCopy(ren, screen_t_l2, nullptr, &DestR);

}
