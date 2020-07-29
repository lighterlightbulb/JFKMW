#pragma once

int DRAW_SIZE_X = 48;
int DRAW_SIZE_Y = 32;

int camera_x = 1;
int camera_y = 1;
int picked_tile = 0x25;
int picked_sprite = -1;
int mouse_x, mouse_y;
int block_size = 0; //Placement size
int updates = 0;
bool snap = false;
double stick_x = 0.0;
double stick_y = 0.0;
bool mouse_l = false;
bool mouse_r = false;
bool mouse_p = false;
bool mouse_l_old = false;

bool input_down = false;
bool input_up = false;
bool input_left = false;
bool input_right = false;
bool input_del = false;

int frame = 0;
int just_dropped = 0;


class button
{
public:
	int x = 0;
	int b = 0;
	bool pressed = false;
	bool pressed_old = false;
	bool pressed_look = false;

	void* onButtonDown;
	void* onButtonUp;
};

button buttons[9];

#define color_FF 0xFF403030*2
#define color_F0 0xFF2B1A1A*2
#define color_E3 0xFF281717*2
#define color_A0 0xFF241414*2
#define color_69 0xFF000000
#define color_BG 0xFF3D2525


void draw_button(int x, int b, bool pressed)
{
	SDL_Rect rect;
	rect.x = x; rect.w = 23;
	rect.y = 6; rect.h = 22;
	SDL_FillRect(screen_s, &rect, pressed ? color_FF : color_69);

	rect.x = x; rect.w = 22;
	rect.y = 6; rect.h = 21;
	SDL_FillRect(screen_s, &rect, pressed ? color_69 : color_FF);

	rect.x = x + 1; rect.w = 21;
	rect.y = 7; rect.h = 20;
	SDL_FillRect(screen_s, &rect, pressed ? color_E3 : color_A0);

	rect.x = x + 1; rect.w = 20;
	rect.y = 7; rect.h = 19;
	SDL_FillRect(screen_s, &rect, pressed ? color_A0 : color_E3);


	for (int px = 1; px < 17; px++) {
		for (int py = 1; py < 17; py++) {

			Color& C = MAP16_IMG[256 + px + b * 16][py + 32];
			if (!(C.r == 0 && C.g == 255 && C.b == 255))
			{
				draw_pixel_to_surface(
					3 + x + px - 1,
					-23 + py - 1,
					C.r, C.g, C.b);
			}
		}
	}
}

void draw_to_screen()
{
	DRAW_SIZE_X = p_w / 16;
	DRAW_SIZE_Y = (p_h - 32) / 16;
	DRAW_SIZE_Y++;
	frame++;
	SDL_UnlockSurface(screen_s);

	SDL_Rect rect;


	if (current_file != "")
	{
		rect.x = 0; rect.w = p_w;
		rect.y = 0; rect.h = p_h;
		SDL_FillRect(screen_s, &rect, 0xff000000);


		int bound_x = DRAW_SIZE_X - 16;
		int bound_y = DRAW_SIZE_Y;
		int size_y = (L_SIZE_Y > bound_y ? bound_y : L_SIZE_Y) * 16;

		rect.x = 0; rect.w = (L_SIZE_X > bound_x ? bound_x : L_SIZE_X) * 16;
		rect.y = p_h - size_y; rect.h = size_y;

		SDL_FillRect(screen_s, &rect, 0xff2D1A00);

		for (int x = 0; x < DRAW_SIZE_X; x++) {
			for (int y = 0; y < DRAW_SIZE_Y; y++) {
				int spx = x << 4;
				int spy = p_h - 48 - (y << 4);

				int p_x = x - 1 + camera_x;
				int p_y = y - 1 + camera_y;

				uint_fast16_t map16tile = 0x25;
				if (x < (DRAW_SIZE_X-16))
				{
					if (p_y < L_SIZE_Y && p_x < L_SIZE_X)
					{
						map16tile = leveldata[p_x + p_y * L_SIZE_X];
					}
				}
				else
				{
					if (y < 32)
					{
						int tx = (x - ((p_w / 16) - 16)) % 16;
						map16tile = (tx) + y * 16;

						spy = (y << 4);
						spx = (p_w - 256 + (tx * 16));
					}
				}

				uint_fast16_t map16real = map16tile;
				//Shit animation code here
				if (map16tile == 0x124 || map16tile == 0x11F)
				{
					map16tile = 0x300 + ((frame / 8) % 4);
				}
				if (map16tile == 0x2B)
				{
					map16tile = 0x310 + ((frame / 8) % 4);
				}
				if (map16tile == 0x12F)
				{
					map16tile = 0x320 + ((frame / 8) % 2);
				}
				if (map16tile == 0x00)
				{
					map16tile = 0x330 + ((frame / 8) % 4);
				}

				if (map16real != 0x25)
				{
					updates += 1;
					uint_fast16_t map16_x = ((map16tile & 0xF) << 4) + (map16tile / 0x200) * 0x100;
					uint_fast16_t map16_y = ((map16tile % 0x200) >> 4) << 4;
					for (int px = 1; px < 17; px++) {
						for (int py = 1; py < 17; py++) {

							Color& C = MAP16_IMG[map16_x + px][map16_y + py];
							if (!(C.r == 0 && C.g == 255 && C.b == 255))
							{
								if (x >= (DRAW_SIZE_X-16) && map16real == picked_tile)
								{
									draw_pixel_to_surface(
										spx + px - 1,
										spy + py - 1,
										255 - C.r, 255 - C.g, 255 - C.b);
								}
								else
								{
									draw_pixel_to_surface(
										spx + px - 1,
										spy + py - 1,
										C.r, C.g, C.b);
								}
							}
						}
					}
				}
			}
		}

		for (int i = 0; i < 512; i++)
		{
			if (Sprite_data[i].exists)
			{
				if (Sprite_data[i].num > 255 || Sprite_data[i].num < 0)
				{
					//cout << "Sprite slot " << i << " is corrupted! Deleting.." << endl;
					Sprite_data[i].exists = false;
					continue;
				}

				int x_pos = 16 + Sprite_data[i].x - camera_x * 16;
				int y_pos = -32 + (p_h - 32) - Sprite_data[i].y + camera_y * 16;
				for (int px = 0; px < 16; px++) {
					for (int py = 1; py < 17; py++) {

						if ((x_pos + px) < (p_w - 256))
						{
							Color& C = MAP16_IMG[256 + px + (Sprite_data[i].type ? 16 : 0)][py + (i == picked_sprite ? 16 : 0)];
							draw_pixel_to_surface(
								x_pos + px,
								y_pos + py - 1,
								C.r, C.g, C.b);
						}
					}
				}
				if (x_pos < (p_w - 256) && x_pos > 0)
				{
					draw_string((Sprite_data[i].type ? "Lua" : "ASM"), x_pos, y_pos, false);
					draw_string(int_to_hex(Sprite_data[i].num, true), x_pos, y_pos + 5, false);
					draw_string(to_string(Sprite_data[i].dir), x_pos, y_pos + 10, false);
				}
			}
		}


		if ((mouse_y >= 0 && picked_sprite < 0) && mouse_x < (p_w - 256))
		{
			int x = (mouse_x >> 4) << 4;
			int y = (((mouse_y - (p_h % 16)) >> 4) << 4) + (p_h % 16);


			for (int px = 1; px < 113; px++) {
				for (int py = 1; py < 113; py++) {

					Color& C = MAP16_IMG[400 + px][py + 112 * block_size];
					if (!(C.r == 0 && C.g == 255 && C.b == 255))
					{
						draw_pixel_to_surface(
							x + px - 49,
							y + py - 49,
							C.r, C.g, C.b);
					}
				}
			}
		}


		int s_x = start_x - camera_x + 1;
		int s_y = 31 - (start_y - camera_y + 1);

		if ((!snap && keyboard_or_controller) && (mouse_x < 512))
		{
			draw_pixel_to_surface(
				mouse_x,
				mouse_y,
				255, 255, 255);
		}
		if (s_x >= 0 && s_y >= 0 && s_x < 32 && s_y < 32)
		{

			SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);
			rect.x = s_x << 4; rect.w = 16;
			rect.y = 32 + (s_y << 4); rect.h = 1;
			SDL_FillRect(screen_s, &rect, 0xFF00FFFF);

			rect.x = s_x << 4; rect.w = 1;
			rect.y = 32 + (s_y << 4); rect.h = 16;
			SDL_FillRect(screen_s, &rect, 0xFF00FFFF);

			rect.x = s_x << 4; rect.w = 16;
			rect.y = 47 + (s_y << 4); rect.h = 1;
			SDL_FillRect(screen_s, &rect, 0xFF00FFFF);

			rect.x = 15 + (s_x << 4); rect.w = 1;
			rect.y = 32 + (s_y << 4); rect.h = 16;
			SDL_FillRect(screen_s, &rect, 0xFF00FFFF);
			SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
		}

		string display1 = "Mouse: " + to_string(mouse_x) + ", " + to_string(mouse_y) + " Camera: " + to_string(camera_x) + ", " + to_string(camera_y);
		string display2 = "FPS " + to_string(int(fps));

		draw_string(display1, 0, 0);
		draw_string(display2, 0, 10);

	}
	else
	{
		rect.x = 0; rect.w = p_w;
		rect.y = 0; rect.h = p_h;
		SDL_FillRect(screen_s, &rect, color_BG);
	}
	/*
		Draw toolbar
	*/

	rect.x = 0; rect.w = p_w;
	rect.y = 0; rect.h = 32;
	SDL_FillRect(screen_s, &rect, color_F0);

	rect.x = 0; rect.w = p_w;
	rect.y = 2; rect.h = 1;
	SDL_FillRect(screen_s, &rect, color_A0);

	rect.x = 0; rect.w = p_w;
	rect.y = 3; rect.h = 1;
	SDL_FillRect(screen_s, &rect, color_FF);

	rect.x = 0; rect.w = p_w;
	rect.y = 30; rect.h = 1;
	SDL_FillRect(screen_s, &rect, color_A0);

	rect.x = 0; rect.w = p_w;
	rect.y = 31; rect.h = 1;
	SDL_FillRect(screen_s, &rect, color_69);

	/*
	
	Draw toolbar buttons

	*/

	for (int i = 0; i < size(buttons); i++)
	{
		draw_button(buttons[i].x, buttons[i].b, buttons[i].pressed_look);
	}

	SDL_LockSurface(screen_s);
	if (screen_s)
	{
		SDL_DestroyTexture(screen_texture);
	}
	screen_texture = SDL_CreateTextureFromSurface(ren, screen_s);
}