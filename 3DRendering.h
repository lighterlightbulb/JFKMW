#pragma once


//ColorRGB

SDL_Surface* surf_screen;
SDL_Texture* tex_screen;
uint_fast16_t render_w = 128;
uint_fast16_t render_h = 112;

#define PI 3.14159265

int p_w;
int p_h;

void draw_pixel(int_fast16_t x1, int_fast16_t y1, uint_fast8_t r, uint_fast8_t g, uint_fast8_t b, SDL_Surface* screen_s)
{
	if (x1 >= 0 && x1 < screen_s->w && y1 >= 0 && y1 < screen_s->h)
	{
		Uint32* p_screen = (Uint32*)screen_s->pixels;
		p_screen += y1 * screen_s->w + x1;
		*p_screen = SDL_MapRGBA(screen_s->format, r, g, b, 255);
	}
}

class PlayerObj
{
public:
	double pos_x, pos_y = 16.0, pos_z;

	double posX = 22, posY = 12;  //x and y start position
	double dirX = -1, dirY = 0; //initial direction vector
	double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane


	void process()
	{

		MPlayer& LocalPlayer = get_mario(SelfPlayerNumber);

		double rotSpeed = 0.1;
		if (LocalPlayer.pad[button_right])
		{
			//both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
			dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
			planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		//rotate to the left
		if (LocalPlayer.pad[button_left])
		{
			//both camera direction and camera plane must be rotated
			double oldDirX = dirX;
			dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
			dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
			double oldPlaneX = planeX;
			planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
			planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}

		if (LocalPlayer.pad[button_b])
		{
			pos_y += 2.0;
		}

		if (LocalPlayer.pad[button_a])
		{
			pos_y -= 2.0;
		}

		double sp_x = dirX * (LocalPlayer.pad[button_up] - LocalPlayer.pad[button_down]);
		double sp_y = dirY * (LocalPlayer.pad[button_up] - LocalPlayer.pad[button_down]);
		pos_x += sp_x;
		pos_z += sp_y;

	}
};

PlayerObj PlayerObjLocal;

/*

	Player

*/

class Cube
{
public:
	double pos_x, pos_y, pos_z, size_x, size_y, size_z;
	uint_fast8_t r, g, b;


	bool check_colision(double x, double y, double z)
	{
		double
			x_point_1 = pos_x - size_x / 2.0, x_point_2 = pos_x + size_x / 2.0,
			y_point_1 = pos_y - size_y / 2.0, y_point_2 = pos_y + size_y / 2.0,
			z_point_1 = pos_z - size_z / 2.0, z_point_2 = pos_z + size_z / 2.0;

		return x > x_point_1 && x < x_point_2 && y > y_point_1 && y < y_point_2 && z > z_point_1 && z < z_point_2;
	}
};

std::vector<Cube> mapD;

void inicializar_map()
{
	mapD.push_back(Cube{ 20.0, 2.5 + 10.0, 0.0, 10.0, 20.0, 10.0, 127, 255, 127 });
	mapD.push_back(Cube{ 20.0, 2.5 + 25.0, 0.0, 4.0, 50.0, 4.0, 255, 127, 127 });
	mapD.push_back(Cube{ 0.0, -4.0, 0.0, 256.0, 10.0, 256.0, 255, 255, 255 });
	mapD.push_back(Cube{ 0.0, 2.5, 30.0, 10.0, 10.0, 10.0, 69, 167, 69 });

	for (int i = 0; i < 6; i++)
	{
		mapD.push_back(Cube{ 20.0, 2.5 + 25.0 + i * 10.0, 40.0, 4.0, 4.0, 4.0, uint_fast8_t(rand() % 255), uint_fast8_t(rand() % 255), uint_fast8_t(rand() % 255) });
	}

	for (int i = 0; i < 20; i++)
	{
		mapD.push_back(Cube{ double(rand() % 100), 2.5, double(rand() % 100), 4.0, 16.0, 4.0, uint_fast8_t(rand() % 255), uint_fast8_t(rand() % 255), uint_fast8_t(rand() % 255) });
	}
}

/*

	
*/

double distance_3d(double x, double x2, double y, double y2, double z, double z2)
{

	return sqrt(pow((x - x2), 2) + pow((y - y2), 2) + pow((z - z2), 2));
}

double distance_2d(double x, double y, double a, double b) {
	return sqrt(pow(x - a, 2) + pow(y - b, 2));
}

void renderizar()
{

	for (uint_fast16_t x = 0; x < render_w; x++)
	{

		double cameraX = 2 * x / (double)render_w - 1; //x-coordinate in camera space

		double d_x = PlayerObjLocal.dirX + PlayerObjLocal.planeX * cameraX;
		double d_z = PlayerObjLocal.dirY + PlayerObjLocal.planeY * cameraX;


		for (uint_fast16_t y = 0; y < render_h; y++)
		{
			double pitch_ray = double(int(y) - int(render_h / 2)) / 16.0;
			double d_y = -pitch_ray / 8.0;

			double pos_x = PlayerObjLocal.pos_x,
				pos_y = PlayerObjLocal.pos_y,
				pos_z = PlayerObjLocal.pos_z;

			bool touched = false; double distance = 0.0;
			while (!touched && distance < 256.0)
			{
				pos_x += d_x; pos_y += d_y; pos_z += d_z;
				distance = distance_3d(
					pos_x, PlayerObjLocal.pos_x,
					pos_y, PlayerObjLocal.pos_y,
					pos_z, PlayerObjLocal.pos_z
				);
				if (pos_y < -2.0)
				{
					double distance_d = distance_2d(pos_x, pos_z, PlayerObjLocal.pos_x, PlayerObjLocal.pos_z);
					double r = double(0) / max(1, 1.0 + abs(distance_d / 128.0));
					double g = double(96) / max(1, 1.0 + abs(distance_d / 128.0));
					double b = double(184) / max(1, 1.0 + abs(distance_d / 128.0));
					draw_pixel(x, y, uint_fast8_t(r), uint_fast8_t(g), uint_fast8_t(b), surf_screen);
					touched = true;
					break;
				}
				for (int i = 0; i < mapD.size(); i++)
				{
					Cube& c = mapD[i];
					if (c.check_colision(pos_x, pos_y, pos_z))
					{
						double distance_d = distance_2d(pos_x, pos_z, PlayerObjLocal.pos_x, PlayerObjLocal.pos_z);
						double r = double(c.r) / max(1, 1.0 + abs(distance_d / 128.0));
						double g = double(c.g) / max(1, 1.0 + abs(distance_d / 128.0));
						double b = double(c.b) / max(1, 1.0 + abs(distance_d / 128.0));
						draw_pixel(x, y, uint_fast8_t(r), uint_fast8_t(g), uint_fast8_t(b), surf_screen);
						touched = true;
						break;
					}
				}
			}


		}
	}

}



void clean_surf()
{

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

	if (surf_screen) {
		SDL_FreeSurface(surf_screen);
	}
	surf_screen = SDL_CreateRGBSurface(0, render_w, render_h, 32,
		rmask, gmask, bmask, amask);
}

bool drawing3D = true;
bool candraw3D = false;
void ThirdLoop()
{
	drawing3D = true;
	clean_surf();
	inicializar_map();

	while (true)
	{
		drawing3D = true;
		Sleep(8);

		
		clean_surf();

		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		PlayerObjLocal.process();

		
		SDL_UnlockSurface(surf_screen);
		renderizar();
		SDL_LockSurface(surf_screen);
		

		candraw3D = false;
		Sleep(8);
		drawing3D = false;

		while (!candraw3D) {
			Sleep(1);
		}
		candraw3D = false;

	}
}

void drawTex()
{
	if (drawing3D) { //Can't draw dis
		return;
	}
	if (surf_screen) {
		if (tex_screen) {
			SDL_DestroyTexture(tex_screen);
		}
		tex_screen = SDL_CreateTextureFromSurface(ren, surf_screen);

		candraw3D = true;
	}
	else
	{
		candraw3D = true;
		return;
	}

}

sf::Thread* t;
void Launch3D()
{
	if (t)
	{
		return;
	}
	t = new sf::Thread(&ThirdLoop); t->launch();
}

void draw3DToScreenSDL()
{
	SDL_Rect DestR;
	DestR.x = sp_offset_x;
	DestR.y = sp_offset_y;
	DestR.w = int_res_x * scale;
	DestR.h = int_res_y * scale;
	SDL_RenderCopy(ren, tex_screen, nullptr, &DestR);


	DestR.x = sp_offset_x + ((int_res_x - 256) * scale) / 2;
	DestR.y = sp_offset_y + ((int_res_y - 224) * scale) / 2;
	DestR.w = 256 * scale;
	DestR.h = 224 * scale;
	SDL_RenderCopy(ren, screen_t_l2, nullptr, &DestR);
}

void DrawExperimental3D()
{
	drawTex();
	draw3DToScreenSDL();
}