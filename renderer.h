#pragma once

#define camBoundX 32.0
#define camBoundY 32.0
int_fast16_t CameraX, CameraY;
uint_fast8_t curr_bg = 0xFF;

void draw_number_hex(int pos_x, int pos_y, uint_fast16_t number, int length)
{
	for (int i = 0; i < length; i++)
	{
		draw8x8_tile_2bpp(pos_x - i*8, pos_y, (number >> (i << 2)) & 0xF, 1, 2);
	}

}

void draw_number_dec(int pos_x, int pos_y, int number)
{
	int length = int(std::to_string(number).length());
	for (int i = 0; i < length; i++)
	{
		draw8x8_tile_2bpp(pos_x - i * 8, pos_y, int(number / pow(10, i)) % 10, 1, 2);
	}

}

void render_oam(uint_fast16_t offset_o = 0, int CameraX = 0, int CameraY = 0)
{
	for (uint_fast16_t i = 0; i < 0x100; i += 8) //Tile, Size, XY (4 bytes), PAL, ANG, in total 8 bytes per entry. 0 to 7.
	{
		uint_fast8_t tile = uint_fast8_t(ASM.Get_Ram(offset_o + i, 1));
		uint_fast8_t size = uint_fast8_t(ASM.Get_Ram(offset_o + i + 1, 1));

		int_fast16_t x_position = int_fast16_t(ASM.Get_Ram(offset_o + i + 2, 2));
		int_fast16_t y_position = int_fast16_t(ASM.Get_Ram(offset_o + i + 4, 2));

		uint_fast8_t pal = uint_fast8_t(ASM.Get_Ram(offset_o + i + 6, 1));

		double angle = (double(ASM.Get_Ram(offset_o + i + 7, 1)) / 256.0) * 360.0;




		if (tile != 0x0 &&
			(x_position - CameraX) > -64 && (x_position - CameraX) < (256 + 64) &&
			(y_position - CameraY) > -64 && (y_position - CameraY) < (224 + 64)			
		)
		{
			if (angle != 0.0 && simulate_SNES_quirks)
			{
				add_to_ticks += rand() % 4;
			}
			draw_tile_custom(x_position - CameraX, 224 - 32 - y_position + CameraY, size, angle, tile, pal);
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

	for (uint_fast16_t i = 0; i < 256; i++)
	{
		palette_array[i] = ServerRAM.RAM[0x3D00 + i] + (ServerRAM.RAM[0x3E00 + i] << 8);
	}

	/*std::copy(ServerRAM.RAM + VRAM_Location,
		ServerRAM.RAM + VRAM_Location + VRAM_Size,
		VRAM);*/
	memcpy(VRAM, &ServerRAM.RAM[VRAM_Location], VRAM_Size * sizeof(uint_fast8_t));



	MPlayer& LocalPlayer = get_mario(SelfPlayerNumber);

	//Camera offsets

	
	CameraX = int_fast16_t(LocalPlayer.CAMERA_X - 120.0);
	CameraY = int_fast16_t(LocalPlayer.CAMERA_Y - 112.0);

	if (CameraX < 0) { CameraX = 0; }
	if (CameraY < 0) { CameraY = 0; }
	if (CameraX > (-256 + mapWidth * 16))
	{
		CameraX = (-256 + mapWidth * 16);
	}
	if (CameraY > (-224 + mapHeight * 16))
	{
		CameraY = (-224 + mapHeight * 16);
	}

	int_fast16_t offsetX = int_fast16_t(CameraX / 16.0);
	uint_fast8_t offsetXPixel = uint_fast8_t(CameraX) % 16;
	int_fast16_t offsetY = int_fast16_t(CameraY / 16.0);
	uint_fast8_t offsetYPixel = uint_fast8_t(CameraY) % 16;

	//Draw BG

	if (ASM.Get_Ram(0x3F05, 1) != curr_bg)
	{
		curr_bg = ASM.Get_Ram(0x3F05, 1);
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
				(-int(double(CameraX) * (double(ServerRAM.RAM[0x3F06]) / 16.0)) % 512) + x*512,
				-256 + (int(double(CameraY) * (double(ServerRAM.RAM[0x3F07]) / 16.0)) % 512) + y*-512);
		}
	}

	// Start rendering, by locking surface.
	SDL_LockSurface(&screen_s_l1);
	SDL_Surface *screen_plane = &screen_s_l1;
	SDL_memset(screen_plane->pixels, 0, screen_plane->h * screen_plane->pitch);

	//Draw OAM (low)

	render_oam(0x300, int(CameraX), int(CameraY));

	//Draw scenery
	uint_fast8_t blocks_on_screen = 0;
	
	for (uint_fast8_t x = 0; x < 17; x++)
	{
		for (uint_fast8_t y = 0; y < 15; y++)
		{
			uint_fast16_t tile = map16_handler.get_tile(x + offsetX, y + offsetY);
			if (tile != 0x25)
			{
				uint_fast16_t entry = tile * tile_table_size;

				//Block 8x8 tiles
				for (uint_fast8_t i = 0; i < 4; i++)
				{
					uint_fast16_t block_index = map16_entries[entry + 1 + (i << 1)] + (map16_entries[entry + (i << 1)] << 8);
					uint_fast8_t index = map16_entries[(i <= 1 ? entry + tile_palette_1 : entry + tile_palette_2)];
					uint_fast8_t block_palette = (i & 1) ? (index & 0xF) : (index >> 4);

					if (block_index != 0xF8)
					{
						blocks_on_screen += 1;
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
	//Draw OAM (priority)

	render_oam(0x200, int(CameraX), int(CameraY));

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


	//Draw Mario

	for (std::list<MPlayer>::iterator item = Mario.begin(); item != Mario.end(); ++item)
	{
		MPlayer& CurrentMario = *item;

		if (CurrentMario.x > (CameraX-camBoundX) && CurrentMario.y > (CameraY-camBoundY) && CurrentMario.x < (CameraX+256+camBoundX) && CurrentMario.y < (CameraY + 224 + camBoundY))
		{
			float is_skidding = 1.f - (float(abs(CurrentMario.SKIDDING))*2.f);

			int offs = 0;
			if ((CurrentMario.to_scale*is_skidding) == -1.f) { offs = -8; }
			if (!CurrentMario.invisible)
			{
				Sprite Mario(path + "Sprites/mario/" + to_string((CurrentMario.player_index - 1) % 2) + "/" + CurrentMario.sprite + ".png", int(CurrentMario.x) + offs - int(CameraX), 224 - 32 - int(CurrentMario.y) + int(CameraY), int(CurrentMario.to_scale*is_skidding) * 24, 32);
			}
		}
	}


	//draw layer 3
	SDL_LockSurface(&screen_s_l2);
	SDL_Surface *screen_plane_sequel = &screen_s_l2;
	SDL_memset(screen_plane_sequel->pixels, 0, screen_plane_sequel->h * screen_plane_sequel->pitch);
	
	//StatusBar
	for (int i = 0; i < 5; i++)
	{
		draw8x8_tile_2bpp(16 + i * 8, 15, 0x30 + i, 0, 2);

	}
	//just debug for now. ignore this ugly shit
	draw8x8_tile_2bpp(24, 15 + 8, 0x26, 1, 2);

	draw_number_hex(40, 15+8, SelfPlayerNumber, 2);
	draw_number_hex(256 - 48, 15, int(LocalPlayer.x), 4);
	draw_number_hex(256 - 88, 15, int(LocalPlayer.y), 4);

	draw_number_hex(256 - 48 - 80, 15, uint_fast16_t(LocalPlayer.Y_SPEED*256.0), 4);
	draw_number_hex(256 - 88 - 80, 15, uint_fast16_t(LocalPlayer.X_SPEED*256.0), 4); //y

	draw8x8_tile_2bpp(8 * 11, 15 + 8, networking ? 0x17 : 0x15, 1, 2);
	draw8x8_tile_2bpp(8 * 10, 15 + 8, 0x50, 1, 2);
	draw8x8_tile_2bpp(8 * 9, 15 + 8, isClient ? 0xC : 0x1C, 1, 2);

	draw_number_hex(256 - 24, 15, blocks_on_screen, 2);

	draw8x8_tile_2bpp(256 - 32, 15 + 8, 0x16, 1, 2); //MS
	draw8x8_tile_2bpp(256 - 24, 15 + 8, 0x1C, 1, 2);
	draw_number_dec(256 - 40, 15 + 8, int(latest_server_response.count()*1000.0));

	draw8x8_tile_2bpp(128, 15 + 8, 0x22, 1, 2); //BY
	draw8x8_tile_2bpp(128 - 8, 15 + 8, 0xb, 1, 2);
	draw_number_dec(128 - 16, 15 + 8, data_size_current);
	data_size_current = 0;

	draw8x8_tile_2bpp(256 - 32 - 48, 15 + 8, 0xF, 1, 2); //FPS
	draw8x8_tile_2bpp(256 - 24 - 48, 15 + 8, 0x19, 1, 2);
	draw8x8_tile_2bpp(256 - 16 - 48, 15 + 8, 0x1C, 1, 2);
	draw_number_dec(256 - 40 - 48, 15 + 8, int(1.0/ (total_time_ticks.count()/1.0) ) );
	

	/*for (uint_fast8_t x_st = 0; x_st < 32; x_st++)
	{
		for (uint_fast8_t y_st = 0; y_st < 32; x_st++)
		{
			draw8x8_tile_2bpp(x_st << 3, y_st << 3, VRAM[0xD000 + (y_st << 5) + x_st], VRAM[0xE000 + (y_st << 5) + x_st], 2);
		}
	}*/

	SDL_UnlockSurface(&screen_s_l2);

	SDL_DestroyTexture(screen_t_l2);
	screen_t_l2 = SDL_CreateTextureFromSurface(ren, &screen_s_l2);
	SDL_RenderCopy(ren, screen_t_l2, nullptr, &DestR);

}
