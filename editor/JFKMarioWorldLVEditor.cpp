
#include <Windows.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <ratio>
#include <chrono>
#include <ctype.h>
#include <cstring>
#include <cassert>
#include <conio.h>

using namespace std;

double window_scale_x = 1;
double window_scale_y = 1;

bool mouse_w_up = false;
bool mouse_w_down = false;

#include <SDL.h>
#undef main


string path = ""; // "E:/JFKMarioWorld/Debug/Editor/";

SDL_GameController* gGameController;
int controller = 0;
bool keyboard_or_controller = false;
double fps = 0;
int frm_counter = 0;
string current_file = "";
string jfkmw_executable = "";
string clevel = "";

#include "ConsoleStuff.h"
#include "SDL_General.h"
#include "LevelData.h"
#include "LevelManager.h"
#include "ImageLoading.h"
#include "Rendering.h"
#include "Controls.h"


int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << red << "Error initializing SDL!" << white << endl;
		return 1;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	cout << cyan << "[SDL] SDL Initialized." << white << endl;

	load_zsnes_font();
	InitializeMap16();
	InitializeConsole();
	InitializeLevel();
	init_controls();
	InitInstalledSprites();

	screen(768, 544, "Solar Energy V1.2.0");

	frame = 0;

	//After a level was loaded
	while (running())
	{
		SDL_GetWindowSize(win, &p_w, &p_h);
		if (p_w != old_p_w || p_h != old_p_h)
		{
			initialize_surface();
			old_p_w = p_w;
			old_p_h = p_h;
		}

		window_scale_x = double(double(p_w) / 768.0);
		window_scale_y = double(double(p_h) / 544.0);



		if (SDL_GetWindowFlags(win) & SDL_WINDOW_INPUT_FOCUS)
		{
			frm_counter++;
			chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
			handle_controls();
			draw_to_screen();

			chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
			chrono::duration<double> total_time_ticks = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
			fps = double(1.0 / (total_time_ticks.count() / 1.0));
		}
		SDL_Rect dest;
		dest.x = 0; dest.y = 0; dest.w = p_w; dest.h = p_h;
		SDL_RenderCopy(ren, screen_texture, NULL, &dest);
		SDL_RenderPresent(ren);

	}
	return 1;
}