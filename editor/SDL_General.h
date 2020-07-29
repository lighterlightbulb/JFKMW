#pragma once
/*
	sdl_general.h

	has functions that are helpful.
*/

SDL_Surface* screen_s; //for drawing
SDL_Texture* screen_texture; //final texture for output



void draw_pixel_to_surface(int x1, int y1, uint_fast8_t r, uint_fast8_t g, uint_fast8_t b)
{
	y1 += 32;
	if (x1 > -1 && x1 < screen_s->w && y1 > -1 && y1 < screen_s->h)
	{
		Uint32* p_screen = (Uint32*)screen_s->pixels;
		p_screen += (y1 * int(screen_s->w)) + x1;
		*p_screen = SDL_MapRGBA(screen_s->format, r, g, b, 255);
	}
}



class ZSNES_letter
{
public:
	bool bits[8][5];
};
ZSNES_letter zsnes_font[0xC0];
void load_zsnes_font()
{
	cout << purple_int << "[ZUI] Loading zfont.txt" << white << endl;
	uint_fast8_t current_letter = 0;
	uint_fast8_t current_offset = 0;
	ifstream cFile(path + "zfont.txt");
	if (cFile.is_open())
	{
		string line;
		while (getline(cFile, line)) {
			if (line[0] == ';') { current_letter += 1; current_offset = 0; continue; }
			if (line.empty()) { continue; }


			ZSNES_letter& curr_l = zsnes_font[current_letter];
			for (uint_fast8_t i = 0; i < 8; i++)
			{
				curr_l.bits[i][current_offset] = line.at(i) == '1';
			}

			current_offset += 1;
		}

		cout << purple_int << "[ZUI] Loaded 0x" << hex << int(current_letter) << dec << " letters." << white << endl;
	}
}

uint_fast8_t char_to_zsnes_font_letter(char l) //use to convert strings
{
	uint_fast8_t new_l = uint_fast8_t(l);
	if (new_l == 0x2E) { return 0x27; }
	if (new_l == 0x3A) { return 0x2D; }
	if (new_l == 0x5F) { return 0x25; }
	if (new_l == 0x2D) { return 0x24; }
	if (new_l > 0x60) { return new_l - 0x57; }
	if (new_l > 0x40) { return new_l - 0x37; }
	if (new_l >= 0x30) { return new_l - 0x30; }
	return 0x8C;
}

void draw_string(string str, int_fast16_t x, int_fast16_t y, bool double_size = true)
{
	for (int i = 0; i < str.size(); i++) {
		uint_fast8_t arr_l = char_to_zsnes_font_letter(str.at(i));
		ZSNES_letter& curr_l = zsnes_font[arr_l];

		for (uint_fast8_t x_l = 0; x_l < 8; x_l++) {
			for (uint_fast8_t y_l = 0; y_l < 5; y_l++) {
				if (curr_l.bits[x_l][y_l])
				{
					uint_fast8_t formula = 255 - y_l * 16;

					if (double_size)
					{
						for (uint_fast8_t px = 0; px < 2; px++) {
							for (uint_fast8_t py = 0; py < 2; py++) {
								draw_pixel_to_surface(px + x + x_l * 2, py + y + y_l * 2, formula, formula, formula);
							}
						}
					}
					else
					{
						draw_pixel_to_surface(x + x_l, y + y_l, formula, formula, formula);
					}
				}
			}
		}
		x += double_size ? 12 : 6;
	}
}

SDL_Window* win; //The window
SDL_Renderer* ren; //The renderer
int p_w = 0;
int p_h = 0;

int old_p_w = 0;
int old_p_h = 0;

void initialize_surface()
{
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
#endif
	//god so fucking retarded

	if (screen_s) {
		SDL_FreeSurface(screen_s);
	}
	screen_s = SDL_CreateRGBSurface(0, p_w, p_h, 32,
		rmask, gmask, bmask, amask);
}

void screen(int width, int height, const std::string& text = "test")
{
	if (win) { SDL_DestroyWindow(win); }

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	win = SDL_CreateWindow(text.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	p_w = width;
	p_h = height;

	if (win == NULL) { std::cout << red << "[SDL] window error : " << SDL_GetError() << std::endl; SDL_Quit(); std::exit(1); }

	flags = SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED;

	ren = SDL_CreateRenderer(win, -1, flags);
	if (ren == NULL) { std::cout << red << "[SDL] renderer error : " << SDL_GetError() << std::endl; SDL_Quit(); std::exit(1); }


	std::cout << blue << "[SDL] screen resolution : " << width << "x" << height << white << std::endl;
	std::cout << blue << "[SDL] current video driver : " << SDL_GetCurrentVideoDriver() << white << std::endl;

	initialize_surface();
}


/*

	check if the game is still running.

*/
SDL_Event event = { 0 };
bool running()
{
	SDL_Delay(5); //So it consumes less processing power
	mouse_w_up = false; mouse_w_down = false;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT) return false;
		if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y > 0) // scroll up
			{
				// Put code for handling "scroll up" here!
				mouse_w_up = true;
			}
			else if (event.wheel.y < 0) // scroll down
			{
				// Put code for handling "scroll down" here!
				mouse_w_down = true;
			}
		}
	}

	return true;
}
