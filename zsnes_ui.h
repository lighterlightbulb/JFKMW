#pragma once

/*
	the zsnes like UI for jfk mario world
*/

#define snow_size 99 //Amount of snow particles.
void draw_pixel_to_surface(uint_fast8_t x1, uint_fast8_t y1, uint_fast8_t r, uint_fast8_t g, uint_fast8_t b, SDL_Surface* screen_s)
{
	if (x1 > -1 && x1 < screen_s->w && y1 > -1 && y1 < screen_s->h)
	{
		Uint32* p_screen = (Uint32*)screen_s->pixels;
		p_screen += y1 * screen_s->w + x1;
		*p_screen = SDL_MapRGBA(screen_s->format, r, g, b, 255);
	}
}

class ZSNES_ui
{
	SDL_Surface* surface; //for drawing
public:
	SDL_Texture* texture; //final texture for output

	//variables
	float snow_x[snow_size];
	float snow_y[snow_size];
	float snow_x_s[snow_size];

	//initializer. done always
	ZSNES_ui()
	{



		clean_surface();
		for (int i = 0; i < snow_size; i++)
		{
			snow_x[i] = float(rand() % 256);
			snow_y[i] = float(rand() % 256);
			snow_x_s[i] = float(1 + (rand() % 3)) / 6.f;
		}
	}

	//initializes the ZSNES surface
	void clean_surface()
	{
		Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
#else
		rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
#endif

		if (surface) {
			SDL_FreeSurface(surface);
		}
		surface = SDL_CreateRGBSurface(0, 256, 224, 32,
			rmask, gmask, bmask, amask);

	}

	//draws rectangle on the surface
	void draw_rectangle(int_fast16_t x_s, int_fast16_t x_e, int_fast16_t y_s, int_fast16_t y_e, uint_fast8_t r, uint_fast8_t g, uint_fast8_t b, uint_fast8_t a = 255)
	{
		SDL_Rect rect;
		rect.x = x_s; rect.w = x_e - x_s;
		rect.y = y_s; rect.h = y_e - y_s;
		SDL_FillRect(surface, &rect, 0x00000000 + r + (g << 8) + (b << 16) + (a << 24));
	}

	//draws a button
	void draw_button(int_fast16_t x_s, int_fast16_t y_s, int_fast16_t x_e, int_fast16_t y_e)
	{
		draw_rectangle(x_s, x_e, y_s, y_e, 88, 84, 80); //bg.

		draw_rectangle(x_s, x_s + 1, y_s, y_e - 1, 104, 100, 96); //left side
		draw_rectangle(x_s + 1, x_e, y_s, y_s + 1, 120, 116, 112); //top side
		draw_rectangle(x_e - 1, x_e, y_s + 1, y_e, 72, 68, 64); //right side
		draw_rectangle(x_s, x_e - 1, y_e - 1, y_e, 56, 52, 48); //bottom side

	}

	//process ZSNES ui
	void process()
	{
		SDL_UnlockSurface(surface);
		SDL_Rect rect;
		rect.x = 0; rect.w = 256;
		rect.y = 0; rect.h = 224;

		draw_rectangle(0, 256, 0, 224, 64, 44, 128, 128);
		draw_rectangle(5, 235, 6, 20, 48, 32, 96, 128);

		for (int i = 0; i < snow_size; i++)
		{
			snow_x[i] += snow_x_s[i] + (float(rand() % 10) - 5.f) / 10.f;
			snow_y[i] += 1.f;

			draw_pixel_to_surface(uint_fast8_t(snow_x[i]), uint_fast8_t(snow_y[i]), 176, 180, 200, surface);
		}

		draw_rectangle(0, 230, 0, 15, 0, 44, 136);
		draw_rectangle(0, 230, 1, 14, 0, 48, 152);
		draw_rectangle(0, 230, 2, 13, 0, 52, 168);
		draw_rectangle(0, 230, 3, 12, 0, 56, 184);
		draw_rectangle(0, 230, 4, 11, 0, 60, 200);
		draw_rectangle(0, 230, 5, 10, 0, 64, 210);
		draw_rectangle(0, 230, 6, 9, 0, 68, 232);
		draw_rectangle(0, 230, 7, 8, 0, 72, 248);




	}

	//finish processing, copy to texture.
	void finish_processing(SDL_Renderer* r)
	{
		SDL_LockSurface(surface);
		if (texture)
		{
			SDL_DestroyTexture(texture);
		}
		texture = SDL_CreateTextureFromSurface(r, surface);
	}
};

ZSNES_ui zsnes_ui;