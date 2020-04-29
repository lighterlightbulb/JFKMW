#pragma once

//ColorRGB

SDL_Surface screen_s_l1;
SDL_Surface screen_s_l2;
SDL_Texture *screen_t_l1;
SDL_Texture *screen_t_l2;



int w; //width of the screen
int h; //height of the screen
SDL_Window*   win; //The window
SDL_Renderer* ren; //The renderer
SDL_Event event = { 0 };


////////////////////////////////////////////////////////////////////////////////
//BASIC SCREEN FUNCTIONS////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//The screen function: sets up the window for 32-bit color graphics.
//Creates a graphical screen of width*height pixels in 32-bit color.
//Set fullscreen to false for a window, or to true for fullscreen output
//text is the caption or title of the window
//also inits SDL therefore you MUST call screen before any other InstantCG or SDL functions
void screen(int width, int height, const std::string& text = "test")
{
	if (win) { SDL_DestroyWindow(win); }

	w = width;
	h = height;

	sp_offset_x = (width / 2) - 128*scale;
	sp_offset_y = (height / 2) - 112*scale;

	int flags = SDL_WINDOW_SHOWN;
	if (fullscreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN;
	}
	if (opengl)
	{
		flags |= SDL_WINDOW_OPENGL;
	}

	win = SDL_CreateWindow(text.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);

	if (win == NULL) { std::cout << cyan << "[SDL] Window error: " << SDL_GetError() << white << std::endl; SDL_Quit(); std::exit(1); }

	flags = 0;
	if (v_sync)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}
	if (renderer_accelerated)
	{
		flags |= SDL_RENDERER_ACCELERATED;
	}
	ren = SDL_CreateRenderer(win, rendering_device, flags);
	if (ren == NULL) { std::cout << cyan << "[SDL] Renderer error: " << SDL_GetError() << white << std::endl; SDL_Quit(); std::exit(1); }


	std::cout << cyan << "[SDL] Initialized window of " << dec << width << "x" << height << " resolution" << white << endl;
	std::cout << cyan << "[SDL] Current video card : " << SDL_GetCurrentVideoDriver() << white << endl;

	//string file = path + "Sprites/icon.ico";
	//SDL_SetWindowIcon(win, IMG_Load(path.c_str()) );

	Uint32 rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	screen_s_l1 = *SDL_CreateRGBSurface(0, 256, 224, 32,
		rmask, gmask, bmask, amask);
	screen_s_l2 = *SDL_CreateRGBSurface(0, 256, 224, 32,
		rmask, gmask, bmask, amask);

}


void end_game()
{
	SDL_DestroyRenderer(ren);
}



void drawRect(int_fast16_t x1, int_fast16_t y1, uint_fast8_t color, SDL_Surface * screen_s)
{
	if (x1 > -1 && x1 < screen_s->w && y1 > -1 && y1 < screen_s->h)
	{
		uint_fast16_t c = palette_array[color];
		Uint32* p_screen = (Uint32*)screen_s->pixels;
		p_screen += y1 * screen_s->w + x1;
		*p_screen = 0xFF000000 + 
					(((c & 0x1F) << 3)) + 
					((((c >> 5) & 0x1F) << 3) << 8) +
					(((c >> 10) << 3) << 16); //SDL_MapRGBA(screen_s->format, r, g, b, 255);
	}
}



void cls()
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
	SDL_RenderClear(ren);
}
void redraw()
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
	SDL_Rect rect;
	if (w != (scale * 256) || h != (scale * 224))
	{
		rect = { 0,0,w, sp_offset_y}; SDL_RenderFillRect(ren, &rect); //simulate v-blank quirk
		rect = { 0,h - sp_offset_y,w,sp_offset_y }; SDL_RenderFillRect(ren, &rect);
		rect = { 0,0,sp_offset_x, h }; SDL_RenderFillRect(ren, &rect);
		rect = { w - sp_offset_x,0,sp_offset_x,h }; SDL_RenderFillRect(ren, &rect);
	}

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderDrawPoint(ren, mouse_x, mouse_y);

	SDL_RenderPresent(ren);
}

bool done(bool delay)
{
	if (delay) SDL_Delay(5); //So it consumes less processing power
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT) return true;
	}
	return quit;
}

void read_from_palette(std::string file)
{
	cout << red << "[PAL] Loading File " << file << white << endl;

	std::ifstream input(file, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	//print
	int curr = 0;
	int curr_p = 0;
	uint8_t current_buffer[2];


	for (auto &v : buffer)
	{
		current_buffer[curr] = uint8_t(v);

		//std::cout << std::hex << current_buffer[curr] << ",";

		if (curr == 1)
		{
			ASM.Write_To_Ram(0x3D00 + curr_p, current_buffer[0], 1);
			ASM.Write_To_Ram(0x3E00 + curr_p, current_buffer[1], 1);
			//ServerRAM.RAM[0x3D00 + curr_p] = current_buffer[0];
			//ServerRAM.RAM[0x3E00 + curr_p] = current_buffer[1];

			curr_p += 1;
			curr = 0;
		}
		else
		{
			curr += 1;
		}

	}

}


void decode_graphics_file(std::string file, int offset = 0)
{
	file = path + file;

	std::ifstream input(file, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	int current_byte = 0;
	for (auto &v : buffer)
	{
		ServerRAM.RAM[VRAM_Location + (offset*4096) + current_byte] = uint_fast8_t(v);
		//cout << std::hex << int(VRAM[(4096 * offset) + current_byte]) << ",";
		current_byte += 1;
	}

	cout << yellow << "[GFX] Loading File " << file << " at 0x" << int_to_hex(offset*4096) << " " << current_byte << " bytes loaded" << white << endl;


}

/*
-------------------------------- Colour possibilities 4BPP -------------------------------- 
0000 = Colour 0 
0001 = Colour 1 
0010 = Colour 2 
0011 = Colour 3 
0100 = Colour 4 
0101 = Colour 5 
0110 = Colour 6 
0111 = Colour 7 
1000 = Colour 8 
1001 = Colour 9 
1010 = Colour A 
1011 = Colour B 
1100 = Colour C 
1101 = Colour D 
1110 = Colour E 
1111 = Colour F

The format is again very similar:

12345678 12345678 12345678 12345678

The one without bold, cursive and underlined is for the lowest bit for each 1x8 row of pixels, the one in bold for the second lowest bit, 
the one in cursive for the second highest bit and the underlined one for the highest bit. Except now you might think, oh. 
So this takes 4 bytes per 1x8 line. Well, that is correct. But do you also think, x0-x3 cover the first 1x8 line? 
If so, then you're slightly off. x0 and x1 are used for the lowest and second-lowest bits, indeed, but the second-highest 
and highest bits of the 8x8 tile are not at x2-x3 - instead, they are at x10-x11. For some reason, the lower bits of the entire 
8x8 tile are handled before the higher bits. Either way, how does this work? Well, assuming everything has colour 0 again, change x0 to $80, 
and the top-leftmost pixel will use colour 1. To $40, it will be the second pixel, etc. etc. We already covered that. If x0 and x1 both are $80, 
it will use colour 3. But here's the trick that makes 4bpp different from 2bpp - it has high bits. It means that if x0, x1 and x10 are $80, the
topleft-most pixel will use colour 7. Just look at the colour possibilities table for 4bpp, and it should make sense - x10 controls the 
second-highest bit, x1 the second-lowest and x0 the lowest. That together forms colour 7. If x0, x1, x10 and x11 are all $80, the top-leftmost 
pixel will use colour F. As you would expect, each 8x8 tile requires 32 bytes.
*/

uint_fast8_t get_bit(uint_fast8_t num, uint_fast8_t k)
{
	return (num >> k) & 1;
}


void draw8x8_tile(int_fast16_t x, int_fast16_t y, uint_fast16_t tile, uint_fast8_t palette)
{
	palette = palette << 4;	tile = tile << 5;
	uint_fast8_t color1 = 0;
	uint_fast8_t i;
	uint_fast8_t index;

	uint_fast8_t graphics_array[32];
	/*std::copy(VRAM + tile,
		VRAM + tile + 32,
		graphics_array);*/
	memcpy(graphics_array, &VRAM[tile], 32 * sizeof(uint_fast8_t));

	for (index = 0; index < 8; index++)
	{
		uint_fast8_t ind = index << 1;
		for (i = 0; i < 8; i++)
		{
			uint_fast16_t y_p = y + index; uint_fast16_t x_p = 7 - i + x;
			if (y_p < 0 || y_p > 223 || x_p < 0 || x_p > 255) {
				continue;
			}
			color1 =
				((graphics_array[0 + ind] >> i) & 1) + 
				(((graphics_array[1 + ind] >> i) & 1) << 1) + 
				(((graphics_array[16 + ind] >> i) & 1) << 2) + 
				(((graphics_array[17 + ind] >> i) & 1) << 3)
				;

			if (color1 != 0)
			{
				//drawPixelScreen(7 - i + x, y + index, color1 + palette);

				uint_fast16_t c = palette_array[color1 + palette];
				Uint32* p_screen = (Uint32*)(&screen_s_l1)->pixels + ((y_p << 8) + x_p);
				*p_screen = 0xFF000000 +
					(((c & 0x1F) << 3)) +
					(((c >> 5) & 0x1F) << 11) +
					((c >> 10) << 19); //SDL_MapRGBA(screen_s->format, r, g, b, 255);
			}
		}
	}

}



void draw8x8_tile_2bpp(uint_fast8_t x, uint_fast8_t y, uint_fast16_t tile, uint_fast8_t palette, uint_fast8_t palette_offs)
{

	uint_fast8_t color1;
	uint8_t graphics_array[2];
	tile = tile << 4;

	for (uint_fast8_t index = 0; index < 16; index += 2)
	{
		memcpy(graphics_array, &VRAM[0XB000 + tile + index], 2 * sizeof(uint_fast8_t));

		for (uint_fast8_t i = 0; i < 8; i++)
		{
			color1 = get_bit(graphics_array[0], i) + (get_bit(graphics_array[1], i)<<1);

			if (color1 != 0)
			{
				drawRect(7 - i + x, y + (index >> 1), color1 + (palette << 4) + (palette_offs << 2), &screen_s_l2);
			}
		}
	}
}



//drawtilecustom

void draw_tile_custom(int_fast16_t x, int_fast16_t y, uint_fast8_t size, double angle, uint_fast16_t tile, uint_fast8_t palette, bool flip)
{
	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
#endif
	uint_fast8_t size_x = (size % 16) + 1;
	uint_fast8_t size_y = (size / 16) + 1;
	SDL_Surface* ti = SDL_CreateRGBSurface(0, size_x * 8, size_y * 8, 32,
		rmask, gmask, bmask, amask);

	//set up
	palette = palette << 4;

	//Ok
	SDL_LockSurface(ti);

	uint_fast8_t color1 = 0;
	for (uint_fast8_t x_b = 0; x_b < size_x; x_b++) //Shitty loop thing sorry
	{
		for (uint_fast8_t y_b = 0; y_b < size_y; y_b++)
		{
			uint_fast8_t x_off_t = x_b << 3;
			uint_fast8_t y_off_t = y_b << 3;

			//cout << dec << int(x_b) << ", " << int(y_b) << endl;
			uint_fast16_t curr_tile = ((tile + x_b + (y_b << 4)) << 5) + 0xC000;
			if (curr_tile > 0xFFE0) //Fix a crash
			{
				curr_tile = 0xFFE0;
			}

			uint_fast8_t graphics_array[32];
			memcpy(graphics_array, &VRAM[curr_tile], 32 * sizeof(uint_fast8_t));

			//drawing a block now.
			for (uint_fast8_t index = 0; index < 8; index++)
			{
				//stupid method here, can be optimized as fuck.
				uint_fast8_t ind = index << 1;

				for (uint_fast8_t i = 0; i < 8; i++)
				{
					color1 =
						((graphics_array[0 + ind] >> i) & 1) +
						(((graphics_array[1 + ind] >> i) & 1) << 1) +
						(((graphics_array[16 + ind] >> i) & 1) << 2) +
						(((graphics_array[17 + ind] >> i) & 1) << 3)
						;

					if (color1 != 0)
					{
						drawRect(7 - i + x_off_t, index + y_off_t, color1 + palette, ti);
					}
				}
			}
		}
	}




	SDL_Texture* drawnTex = SDL_CreateTextureFromSurface(ren, ti);
	SDL_Rect SourceR;
	SourceR.x = (x * scale) + sp_offset_x;
	SourceR.y = (y * scale) + sp_offset_y;
	SourceR.w = (size_x << 3) * scale; //Same bs as above.
	SourceR.h = (size_y << 3) * scale;
	SDL_RendererFlip flip_r = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	//SDL_BlitSurface(ti, NULL, &screen_s_l1, &SourceR);
	//SDL_UnlockSurface(ti);
	//SDL_Rect DestR;
	//DestR.x = x;
	//DestR.y = y;
	//DestR.w = size_x << 3;
	//DestR.h = size_y << 3;
	//SDL_BlitSurface(ti, &DestR, &screen_s_l1, NULL);

	//SDL_BlitScaled(ti, NULL, &screen_s_l1, &DestR);
	//SDL_UnlockSurface(ti);
	SDL_RenderCopyEx(ren, drawnTex, NULL, &SourceR, angle, NULL, flip_r);
	//bofa deez. 
	SDL_FreeSurface(ti);
	SDL_DestroyTexture(drawnTex);
}